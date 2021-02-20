/*
 * Original copyright (c) 2002 Remi Guyomarch <rguyom@pobox.com>
 * Port copyright (c) 2010 Daniel G. Taylor <dan@programmer-art.org>
 * Relicensed to the LGPL with permission from Remi Guyomarch.
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * blur / sharpen filter, ported to FFmpeg from MPlayer
 * libmpcodecs/unsharp.c.
 *
 * This code is based on:
 *
 * An Efficient algorithm for Gaussian blur using finite-state machines
 * Frederick M. Waltz and John W. V. Miller
 *
 * SPIE Conf. on Machine Vision Systems for Inspection and Metrology VII
 * Originally published Boston, Nov 98
 *
 * http://www.engin.umd.umich.edu/~jwvm/ece581/21_GBlur.pdf
 */

#include <nppi.h>
#include <nppdefs.h>

#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/hwcontext.h"
#include "libavutil/hwcontext_cuda_internal.h"

#include "avfilter.h"
#include "config.h"
#include "formats.h"
#include "internal.h"
#include "video.h"


#define MIN_MATRIX_SIZE 3
#define MAX_MATRIX_SIZE 63
#define CUDA_FRAME_ALIGNMENT 256

static const enum AVPixelFormat supported_formats[] = {
    AV_PIX_FMT_YUV420P,
    AV_PIX_FMT_NV12,
    AV_PIX_FMT_YUV444P,
    AV_PIX_FMT_BGR32,
};

static const enum AVPixelFormat deinterleaved_formats[][2] = {
    { AV_PIX_FMT_NV12, AV_PIX_FMT_YUV420P },
};

enum UnsharpStage {
    STAGE_DEINTERLEAVE,
    STAGE_CONTRAST,
    STAGE_UNSHARP,
    STAGE_INTERLEAVE,
    STAGE_NB,
};

typedef struct NPPUnsharpStageContext {
    int stage_needed;
    enum AVPixelFormat in_fmt;
    enum AVPixelFormat out_fmt;

    struct {
        int width;
        int height;
    } planes[3];

    AVBufferRef *frames_ctx;
    AVFrame     *frame;
} NPPUnsharpStageContext;

typedef struct NPPUnsharpContext {
    const AVClass *class;
    int lmsize_xy;
    float lamount;
    float contrast;

    NppiMaskSize mask;

    NPPUnsharpStageContext stages[STAGE_NB];
    AVFrame *tmp_frame;
    int passthrough;

    uint8_t *tmp_buf0;
    uint8_t *tmp_buf1;
} NPPUnsharpContext;

static NppiMaskSize get_mask_size(int xy) {
    NppiMaskSize mask;
    if (xy == 3) {
        mask = NPP_MASK_SIZE_3_X_3;
    } else if (xy == 5) {
        mask = NPP_MASK_SIZE_5_X_5;
    } else if (xy == 7) {
        mask = NPP_MASK_SIZE_7_X_7;
    } else if (xy == 9) {
        mask = NPP_MASK_SIZE_9_X_9;
    } else if (xy == 11) {
        mask = NPP_MASK_SIZE_11_X_11;
    } else if (xy == 13) {
        mask = NPP_MASK_SIZE_13_X_13;
    } else if (xy == 15) {
        mask = NPP_MASK_SIZE_15_X_15;
    } else {
        mask = NPP_MASK_SIZE_5_X_5;
    }

    return mask;
}

static av_cold int npp_unsharp_init(AVFilterContext *ctx)
{
    NPPUnsharpContext *s = ctx->priv;

    for (int i = 0; i < FF_ARRAY_ELEMS(s->stages); i++) {
        s->stages[i].frame = av_frame_alloc();
        if (!s->stages[i].frame)
            return AVERROR(ENOMEM);
    }

    s->tmp_frame = av_frame_alloc();
    if (!s->tmp_frame)
        return AVERROR(ENOMEM);

    s->tmp_buf0 = NULL;
    s->tmp_buf1 = NULL;

    s->mask = get_mask_size(s->lmsize_xy);

    return 0;
}

static av_cold void npp_unsharp_uninit(AVFilterContext *ctx)
{
    NPPUnsharpContext *s = ctx->priv;

    for (int i = 0; i < FF_ARRAY_ELEMS(s->stages); i++) {
        av_frame_free(&s->stages[i].frame);
        av_buffer_unref(&s->stages[i].frames_ctx);
    }

    av_frame_free(&s->tmp_frame);

    if(s->tmp_buf0 && s->tmp_buf1) {
        AVHWFramesContext *in_frames_ctx;
        AVCUDADeviceContext *device_hwctx;
        CUresult err;
        CUcontext dummy;

        //todo: tmp buffer is not released due to ctx->inputs[0] is a null-pointer.
        if (ctx->inputs[0]) {
            in_frames_ctx = (AVHWFramesContext *)ctx->inputs[0]->hw_frames_ctx->data;
            device_hwctx = in_frames_ctx->device_ctx->hwctx;
            err = device_hwctx->internal->cuda_dl->cuCtxPushCurrent(device_hwctx->cuda_ctx);
            if (err != CUDA_SUCCESS)
                return;

            device_hwctx->internal->cuda_dl->cuMemFree((CUdeviceptr)s->tmp_buf0);
            device_hwctx->internal->cuda_dl->cuMemFree((CUdeviceptr)s->tmp_buf1);
            device_hwctx->internal->cuda_dl->cuCtxPopCurrent(&dummy);
        }
    }
}

static int npp_unsharp_query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_CUDA, AV_PIX_FMT_NONE
    };

    AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}

static int init_stage(NPPUnsharpStageContext *stage, AVBufferRef *device_ctx)
{
    AVBufferRef *out_ref = NULL;
    AVHWFramesContext *out_ctx;
    int in_sw, in_sh, out_sw, out_sh;
    int ret, i;

    av_pix_fmt_get_chroma_sub_sample(stage->in_fmt,  &in_sw,  &in_sh);
    av_pix_fmt_get_chroma_sub_sample(stage->out_fmt, &out_sw, &out_sh);

    for (i = 1; i < FF_ARRAY_ELEMS(stage->planes); i++) {
        stage->planes[i].width   = stage->planes[0].width   >> in_sw;
        stage->planes[i].height  = stage->planes[0].height  >> in_sh;
    }

    out_ref = av_hwframe_ctx_alloc(device_ctx);
    if (!out_ref)
        return AVERROR(ENOMEM);
    out_ctx = (AVHWFramesContext*)out_ref->data;

    out_ctx->format    = AV_PIX_FMT_CUDA;
    out_ctx->sw_format = stage->out_fmt;
    out_ctx->width     = FFALIGN(stage->planes[0].width,  32);
    out_ctx->height    = FFALIGN(stage->planes[0].height, 32);

    ret = av_hwframe_ctx_init(out_ref);
    if (ret < 0)
        goto fail;

    av_frame_unref(stage->frame);
    ret = av_hwframe_get_buffer(out_ref, stage->frame, 0);
    if (ret < 0)
        goto fail;

    stage->frame->width  = stage->planes[0].width;
    stage->frame->height = stage->planes[0].height;

    av_buffer_unref(&stage->frames_ctx);
    stage->frames_ctx = out_ref;

    return 0;
fail:
    av_buffer_unref(&out_ref);
    return ret;
}

static int format_is_supported(enum AVPixelFormat fmt)
{
    int i;

    for (i = 0; i < FF_ARRAY_ELEMS(supported_formats); i++)
        if (supported_formats[i] == fmt)
            return 1;
    return 0;
}

static enum AVPixelFormat get_deinterleaved_format(enum AVPixelFormat fmt)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(fmt);
    int i, planes;

    planes = av_pix_fmt_count_planes(fmt);
    if (planes == desc->nb_components)
        return fmt;
    for (i = 0; i < FF_ARRAY_ELEMS(deinterleaved_formats); i++)
        if (deinterleaved_formats[i][0] == fmt)
            return deinterleaved_formats[i][1];
    return AV_PIX_FMT_NONE;
}

static int init_processing_chain(AVFilterContext *ctx, AVFilterLink *inlink)
{
    NPPUnsharpContext *s = ctx->priv;

    AVHWFramesContext *in_frames_ctx;

    enum AVPixelFormat in_format;
    enum AVPixelFormat out_format;
    enum AVPixelFormat in_deinterleaved_format;
    enum AVPixelFormat out_deinterleaved_format;

    int i, ret, last_stage = -1;

    /* check that we have a hw context */
    if (!ctx->inputs[0]->hw_frames_ctx) {
        av_log(ctx, AV_LOG_ERROR, "No hw context provided on input\n");
        return AVERROR(EINVAL);
    }
    in_frames_ctx = (AVHWFramesContext*)ctx->inputs[0]->hw_frames_ctx->data;
    in_format     = in_frames_ctx->sw_format;
    out_format    = in_format;

    if (!format_is_supported(in_format)) {
        av_log(ctx, AV_LOG_ERROR, "Unsupported input format: %s\n",
               av_get_pix_fmt_name(in_format));
        return AVERROR(ENOSYS);
    }

    in_deinterleaved_format  = get_deinterleaved_format(in_format);
    out_deinterleaved_format = get_deinterleaved_format(out_format);
    if (in_deinterleaved_format  == AV_PIX_FMT_NONE
        || out_deinterleaved_format  == AV_PIX_FMT_NONE) {
        av_log(ctx, AV_LOG_ERROR, "in_deinterleaved_format none!\n");
        return AVERROR_BUG;
    }

    if (!s->passthrough) {
        if (in_format != in_deinterleaved_format)
            s->stages[STAGE_DEINTERLEAVE].stage_needed = 1;
        if (out_format != out_deinterleaved_format)
            s->stages[STAGE_INTERLEAVE].stage_needed = 1;

        s->stages[STAGE_CONTRAST].stage_needed = 1;
        s->stages[STAGE_UNSHARP].stage_needed = 1;
    }

    s->stages[STAGE_DEINTERLEAVE].in_fmt           = in_format;
    s->stages[STAGE_DEINTERLEAVE].out_fmt          = in_deinterleaved_format;
    s->stages[STAGE_DEINTERLEAVE].planes[0].width  = inlink->w;
    s->stages[STAGE_DEINTERLEAVE].planes[0].height = inlink->h;

    s->stages[STAGE_CONTRAST].in_fmt            = in_deinterleaved_format;
    s->stages[STAGE_CONTRAST].out_fmt           = out_deinterleaved_format;
    s->stages[STAGE_CONTRAST].planes[0].width   = inlink->w;
    s->stages[STAGE_CONTRAST].planes[0].height  = inlink->h;

    s->stages[STAGE_UNSHARP].in_fmt            = in_deinterleaved_format;
    s->stages[STAGE_UNSHARP].out_fmt           = out_deinterleaved_format;
    s->stages[STAGE_UNSHARP].planes[0].width   = inlink->w;
    s->stages[STAGE_UNSHARP].planes[0].height  = inlink->h;

    s->stages[STAGE_INTERLEAVE].in_fmt           = out_deinterleaved_format;
    s->stages[STAGE_INTERLEAVE].out_fmt          = out_format;
    s->stages[STAGE_INTERLEAVE].planes[0].width  = inlink->w;
    s->stages[STAGE_INTERLEAVE].planes[0].height = inlink->h;

    /* init the hardware contexts */
    for (i = 0; i < FF_ARRAY_ELEMS(s->stages); i++) {
        if (!s->stages[i].stage_needed)
            continue;

        ret = init_stage(&s->stages[i], in_frames_ctx->device_ref);
        if (ret < 0)
            return ret;

        last_stage = i;
    }

    if (last_stage >= 0)
        ctx->outputs[0]->hw_frames_ctx = av_buffer_ref(s->stages[last_stage].frames_ctx);
    else
        ctx->outputs[0]->hw_frames_ctx = av_buffer_ref(ctx->inputs[0]->hw_frames_ctx);

    if (!ctx->outputs[0]->hw_frames_ctx)
        return AVERROR(ENOMEM);

    {
        CUdeviceptr data;
        CUcontext dummy;
        AVCUDADeviceContext *device_hwctx = in_frames_ctx->device_ctx->hwctx;

        CUresult err = device_hwctx->internal->cuda_dl->cuCtxPushCurrent(device_hwctx->cuda_ctx);
        if (err != CUDA_SUCCESS)
            return AVERROR_UNKNOWN;

        err = device_hwctx->internal->cuda_dl->cuMemAlloc(&data, FFALIGN(inlink->w, CUDA_FRAME_ALIGNMENT) * inlink->h * 4);
        if (err != CUDA_SUCCESS) goto Error;
        s->tmp_buf0 = (uint8_t *)data;

        err = device_hwctx->internal->cuda_dl->cuMemAlloc(&data, FFALIGN(inlink->w, CUDA_FRAME_ALIGNMENT) * inlink->h * 4);
        if (err != CUDA_SUCCESS) goto Error;
        s->tmp_buf1 = (uint8_t *)data;

Error:
        device_hwctx->internal->cuda_dl->cuCtxPopCurrent(&dummy);
        if (err < 0)
            return AVERROR_UNKNOWN;
    }

    return 0;
}

static int nppunsharp_config_props(AVFilterLink *link)
{
    //NPPUnsharpContext *s = link->dst->priv;
    AVFilterContext *ctx = link->src;
    AVFilterLink *inlink = link->src->inputs[0];
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(link->format);

    int ret = init_processing_chain(ctx, inlink);
    if (ret < 0)
        return ret;

    return 0;
}

static int npp_deinterleave(AVFilterContext *ctx, NPPUnsharpStageContext *stage,
                                 AVFrame *out, AVFrame *in)
{
    AVHWFramesContext *in_frames_ctx = (AVHWFramesContext*)in->hw_frames_ctx->data;
    NppStatus err;

    switch (in_frames_ctx->sw_format) {
    case AV_PIX_FMT_NV12:
        err = nppiYCbCr420_8u_P2P3R(in->data[0], in->linesize[0],
                                    in->data[1], in->linesize[1],
                                    out->data, out->linesize,
                                    (NppiSize){ in->width, in->height });
        break;
    default:
        return AVERROR_BUG;
    }
    if (err != NPP_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "NPP deinterleave error: %d\n", err);
        return AVERROR_UNKNOWN;
    }

    return 0;
}

#define CHECK_RUN(func, param) \
err = func param; \
if (err != NPP_SUCCESS) { \
    av_log(ctx, AV_LOG_ERROR, "%s: error = %d\n", #func, err); \
    return AVERROR_UNKNOWN; \
}

static void dump_frame(AVFrame *src, const char *name) {
    AVFrame *frame;

    if (!src || !src->data)
        return;

    frame = av_frame_alloc();
    if (src->format == AV_PIX_FMT_CUDA) {
        frame->format = AV_PIX_FMT_YUV420P;
        if ((av_hwframe_transfer_data(frame, src, 0)) < 0) {
            fprintf(stderr, "hw frame transfer failed!\n");
        }

        av_frame_copy_props(frame, src);
    } else {
        av_frame_ref(frame, src);
    }

    if (frame->format == AV_PIX_FMT_YUV420P) {
        FILE *fp = fopen(name, "wb");
        if (fp) {
            fwrite(frame->data[0], 1, frame->width*frame->height, fp);
            fwrite(frame->data[1], 1, frame->width*frame->height/4, fp);
            fwrite(frame->data[2], 1, frame->width*frame->height/4, fp);
            fclose(fp);
            fp = NULL;
        }
    }

    av_frame_free(&frame);
}

static int npp_contrast(AVFilterContext *ctx, NPPUnsharpStageContext *stage,
                        AVFrame *out, AVFrame *in)
{
    NPPUnsharpContext *s = ctx->priv;
    NppStatus err;
    float offset = 127 -  128 * s->contrast;
    int i;

    for (i = 0; i < FF_ARRAY_ELEMS(stage->planes) && i < FF_ARRAY_ELEMS(in->data) && in->data[i]; i++) {
        int w = stage->planes[i].width;
        int h = stage->planes[i].height;
        int aligned_w = FFALIGN(w, CUDA_FRAME_ALIGNMENT) << 2;

        if (i > 0) {
            CHECK_RUN (nppiCopy_8u_C1R,
                (in->data[i], in->linesize[i], out->data[i], out->linesize[i], (NppiSize){w, h}));

            continue;
        }

        CHECK_RUN (nppiConvert_8u32f_C1R,
            (in->data[i], in->linesize[i], (Npp32f *)s->tmp_buf0, aligned_w, (NppiSize){ w, h }));

        CHECK_RUN (nppiMulC_32f_C1IR,
            (s->contrast, (Npp32f *)s->tmp_buf0, aligned_w, (NppiSize){w, h}));

        CHECK_RUN (nppiAddC_32f_C1IR,
            (offset, (Npp32f *)s->tmp_buf0, aligned_w, (NppiSize){w, h}));

        CHECK_RUN (nppiConvert_32f8u_C1R,
            ((Npp32f *)s->tmp_buf0, aligned_w, out->data[i], out->linesize[i], (NppiSize){ w, h }, NPP_RND_NEAR));
    }

    return 0;
}

static int npp_unsharp(AVFilterContext *ctx, NPPUnsharpStageContext *stage,
                           AVFrame *out, AVFrame *in)
{
    //AVFilterLink *inlink = ctx->inputs[0];
    NPPUnsharpContext *s = ctx->priv;
    NppStatus err;
    int i;

    for (i = 0; i < FF_ARRAY_ELEMS(stage->planes) && i < FF_ARRAY_ELEMS(in->data) && in->data[i]; i++) {
        int w = stage->planes[i].width;
        int h = stage->planes[i].height;
        int aligned_w = FFALIGN(w, CUDA_FRAME_ALIGNMENT) << 2;

        if (i > 0) {
            CHECK_RUN (nppiAddC_8u_C1RSfs,
                (in->data[i], in->linesize[i], 0, out->data[i], out->linesize[i], (NppiSize){w, h}, 0));

            continue;
        }

        CHECK_RUN (nppiConvert_8u32f_C1R,
            (in->data[i], in->linesize[i], (Npp32f*)s->tmp_buf0, aligned_w, (NppiSize){w, h}));

        CHECK_RUN (nppiFilterGaussBorder_32f_C1R,
            ((Npp32f*)s->tmp_buf0, aligned_w, (NppiSize){w, h}, (NppiPoint){0, 0},
            s->tmp_buf1, aligned_w, (NppiSize){w, h}, s->mask, NPP_BORDER_REPLICATE));

        CHECK_RUN (nppiSub_32f_C1IR,
            (s->tmp_buf0, aligned_w, s->tmp_buf1, aligned_w, (NppiSize){w, h}));

        CHECK_RUN (nppiMulC_32f_C1IR,
            (s->lamount, (Npp32f*)s->tmp_buf1, aligned_w, (NppiSize){w, h}));

        CHECK_RUN (nppiSub_32f_C1IR,
            (s->tmp_buf1, aligned_w, s->tmp_buf0, aligned_w, (NppiSize){w, h}));

        CHECK_RUN (nppiConvert_32f8u_C1R,
            ((Npp32f*)s->tmp_buf0, aligned_w, out->data[i], out->linesize[i], (NppiSize){w, h}, NPP_RND_NEAR));
    }

    return 0;
}

static int npp_interleave(AVFilterContext *ctx, NPPUnsharpStageContext *stage,
                               AVFrame *out, AVFrame *in)
{
    AVHWFramesContext *out_frames_ctx = (AVHWFramesContext*)out->hw_frames_ctx->data;
    NppStatus err;

    switch (out_frames_ctx->sw_format) {
    case AV_PIX_FMT_NV12:
        err = nppiYCbCr420_8u_P3P2R((const uint8_t**)in->data,
                                    in->linesize,
                                    out->data[0], out->linesize[0],
                                    out->data[1], out->linesize[1],
                                    (NppiSize){ in->width, in->height });
        break;
    default:
        return AVERROR_BUG;
    }
    if (err != NPP_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "NPP deinterleave error: %d\n", err);
        return AVERROR_UNKNOWN;
    }
    return 0;
}

static int (*const nppunsharp_process[])(AVFilterContext *ctx, NPPUnsharpStageContext *stage,
                                       AVFrame *out, AVFrame *in) = {
    [STAGE_DEINTERLEAVE] = npp_deinterleave,
    [STAGE_CONTRAST]     = npp_contrast,
    [STAGE_UNSHARP]      = npp_unsharp,
    [STAGE_INTERLEAVE]   = npp_interleave,
};

static int npp_unsharp_process(AVFilterContext *ctx, AVFrame *out, AVFrame *in)
{
    NPPUnsharpContext *s = ctx->priv;
    AVFrame *src = in;
    int i, ret, last_stage = -1;

    for (i = 0; i < FF_ARRAY_ELEMS(s->stages); i++) {
        if (!s->stages[i].stage_needed)
            continue;

        ret = nppunsharp_process[i](ctx, &s->stages[i], s->stages[i].frame, src);
        if (ret < 0)
            return ret;

        src        = s->stages[i].frame;
        last_stage = i;
    }

    if (last_stage < 0)
        return AVERROR_BUG;
    ret = av_hwframe_get_buffer(src->hw_frames_ctx, s->tmp_frame, 0);
    if (ret < 0)
        return ret;

    av_frame_move_ref(out, src);
    av_frame_move_ref(src, s->tmp_frame);

    ret = av_frame_copy_props(out, in);
    if (ret < 0)
        return ret;

    return 0;
}

static int nppunsharp_filter_frame(AVFilterLink *link, AVFrame *in)
{
    AVFilterContext *ctx = link->dst;
    NPPUnsharpContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVHWFramesContext *frames_ctx = (AVHWFramesContext*)outlink->hw_frames_ctx->data;
    AVCUDADeviceContext *device_hwctx = frames_ctx->device_ctx->hwctx;

    AVFrame *out = NULL;
    CUresult err;
    CUcontext dummy;
    int ret = 0;

    if (s->passthrough)
        return ff_filter_frame(outlink, in);

    out = av_frame_alloc();
    if (!out) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    err = device_hwctx->internal->cuda_dl->cuCtxPushCurrent(device_hwctx->cuda_ctx);
    if (err != CUDA_SUCCESS) {
        ret = AVERROR_UNKNOWN;
        goto fail;
    }

    ret = npp_unsharp_process(ctx, out, in);

    device_hwctx->internal->cuda_dl->cuCtxPopCurrent(&dummy);
    if (ret < 0)
        goto fail;

    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
fail:
    av_frame_free(&in);
    av_frame_free(&out);
    return ret;
}

#define OFFSET(x) offsetof(NPPUnsharpContext, x)
#define FLAGS AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_VIDEO_PARAM
#define MIN_SIZE 3
#define MAX_SIZE 15
static const AVOption npp_unsharp_options[] = {
    { "luma_msize_xy",   "set luma matrix size",  OFFSET(lmsize_xy), AV_OPT_TYPE_INT,   { .i64 = 5 }, MIN_SIZE, MAX_SIZE, FLAGS },
    { "lxy",             "set luma matrix size",  OFFSET(lmsize_xy), AV_OPT_TYPE_INT,   { .i64 = 5 }, MIN_SIZE, MAX_SIZE, FLAGS },
    { "luma_amount",    "set luma effect strength",          OFFSET(lamount),  AV_OPT_TYPE_FLOAT, { .dbl = 1 },       -2,        5, FLAGS },
    { "la",             "set luma effect strength",          OFFSET(lamount),  AV_OPT_TYPE_FLOAT, { .dbl = 1 },       -2,        5, FLAGS },
    { "contrast",       "set contrast effect strength",      OFFSET(contrast),  AV_OPT_TYPE_FLOAT, { .dbl = 1 },      -2,        5, FLAGS },
    { NULL }
};

AVFILTER_DEFINE_CLASS(npp_unsharp);

static const AVFilterPad avfilter_vf_unsharp_npp_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = nppunsharp_filter_frame,
    },
    { NULL }
};

static const AVFilterPad avfilter_vf_unsharp_npp_outputs[] = {
    {
        .name = "default",
        .type = AVMEDIA_TYPE_VIDEO,
        .config_props = nppunsharp_config_props,
    },
    { NULL }
};

AVFilter ff_vf_unsharp_npp = {
    .name          = "unsharp_npp",
    .description   = NULL_IF_CONFIG_SMALL("NVIDIA Performance Primitives video Sharpen or blur"),
    .priv_size     = sizeof(NPPUnsharpContext),
    .priv_class    = &npp_unsharp_class,
    .init          = npp_unsharp_init,
    .uninit        = npp_unsharp_uninit,
    .query_formats = npp_unsharp_query_formats,
    .inputs        = avfilter_vf_unsharp_npp_inputs,
    .outputs       = avfilter_vf_unsharp_npp_outputs,
    .flags_internal= FF_FILTER_FLAG_HWFRAME_AWARE,
};
