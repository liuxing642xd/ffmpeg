/*
 * Copyright (c) 2007 Bobby Bingham
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
 * video jnd process filter
 */

#include <stdio.h>

#include "avfilter.h"
#include "formats.h"
#include "internal.h"
#include "video.h"
#include "libavutil/eval.h"
#include "libavutil/avstring.h"
#include "libavutil/internal.h"
#include "libavutil/libm.h"
//#include "libavutil/imgutils.h"
//#include "libavutil/mathematics.h"
#include "libavutil/opt.h"
#include "jnd_filter_wrapper.h"

//static const char *const var_names[] = {
//    "qp_expr", 
//    NULL
//};
//
//enum var_name {
//    VAR_QP,
//    VAR_VARS_NB
//};

typedef struct JndContext {
    const AVClass *class;
    int qp;
    int nthreads;
    //char *qp_expr;
    //AVExpr *qp_pexpr,
    //double var_values[VAR_VARS_NB];
    void *jnd_filter;
    uint8_t *tmp;
} JndContext;

static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;

    static const enum AVPixelFormat jnd_fmts[] = {
        AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE
    };

    if(!(formats = ff_make_format_list(jnd_fmts))) {
        return AVERROR(ENOMEM);
    }

    return ff_set_common_formats(ctx, formats);
}

static av_cold void uninit(AVFilterContext *ctx)
{
    JndContext *s = ctx->priv;

    //av_expr_free(s->qp_pexpr);
    //s->qp_pexpr = NULL;
    DestoryJndFilter(s->jnd_filter);
    if(s->tmp)
        av_freep(&s->tmp);
}

static int config_input(AVFilterLink *link)
{
    //AVFilterContext *ctx = link->dst;
    //JndContext *s = ctx->priv;
    //int ret;

    //av_expr_free(s->qp_pexpr);
    //s->qp_pexpr = NULL;
    //if ((ret = av_expr_parse(&s->qp_pexpr, s->qp_expr, var_names,
    //                         NULL, NULL, NULL, NULL, 0, ctx)) < 0)
    //    return AVERROR(EINVAL);

    //av_log(ctx, AV_LOG_VERBOSE, "w:%d h:%d sar:%d/%d -> w:%d h:%d sar:%d/%d\n",
    //       link->w, link->h, link->sample_aspect_ratio.num, link->sample_aspect_ratio.den,
    //       s->w, s->h, s->out_sar.num, s->out_sar.den);
    return 0;
}

static int config_output(AVFilterLink *link)
{
    AVFilterContext *ctx = link->src;
    JndContext *s = ctx->priv;

    link->w = ctx->inputs[0]->w;
    link->h = ctx->inputs[0]->h;
    link->sample_aspect_ratio = ctx->inputs[0]->sample_aspect_ratio;

    s->tmp = av_malloc(FFALIGN(link->w, 32) * link->h);
    if(!s->tmp)
        return AVERROR(ENOMEM);

    av_log(ctx, AV_LOG_INFO, "jnd apply paras qp: %d.\n", s->qp);
    s->jnd_filter = CreateAndInitJndFilter(s->qp, s->nthreads);
    if(!s->jnd_filter) {
        av_freep(&s->tmp);
        return AVERROR(ENOMEM);
    }

    return 0;
}

static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    AVFilterContext *ctx = link->dst;
    JndContext *s = ctx->priv;

    // only copy Y plane
    memcpy(s->tmp, frame->data[0], frame->linesize[0] * frame->height);
    JndFilterProcess(s->jnd_filter, s->tmp, frame->data[0], frame->width, frame->height, frame->linesize[0]);

    return ff_filter_frame(link->dst->outputs[0], frame);
}

static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    JndContext *s = ctx->priv;
    int ret = 0;

    if (!strcmp(cmd, "qp")) {
        int old_qp = s->qp;
        
        av_opt_set(s, cmd, args, 0);
        if(old_qp != s->qp) {
            av_log(ctx, AV_LOG_INFO, "jnd update paras qp: %d.\n", s->qp);
            JndFilterUpdate(s->jnd_filter, s->qp);
        }
    } else
        ret = AVERROR(ENOSYS);

    return ret;
}

#define OFFSET(x) offsetof(JndContext, x)
#define FLAGS AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_VIDEO_PARAM

static const AVOption jnd_options[] = {
    { "qp",   "set qp level paras",   OFFSET(qp),      AV_OPT_TYPE_INT, { .i64 = 27 }, 0, INT_MAX, .flags = FLAGS },
    { "th",   "set thread count",     OFFSET(nthreads), AV_OPT_TYPE_INT, { .i64 = 1 },  0, INT_MAX, .flags = FLAGS },
    { NULL }
};

AVFILTER_DEFINE_CLASS(jnd);

static const AVFilterPad avfilter_vf_jnd_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = filter_frame,
        .config_props = config_input,
    },
    { NULL }
};

static const AVFilterPad avfilter_vf_jnd_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_output,
    },
    { NULL }
};

AVFilter ff_vf_jnd = {
    .name            = "jnd",
    .description     = NULL_IF_CONFIG_SMALL("jnd process the input video."),
    .priv_size       = sizeof(JndContext),
    .priv_class      = &jnd_class,
    .query_formats   = query_formats,
    .uninit          = uninit,
    .inputs          = avfilter_vf_jnd_inputs,
    .outputs         = avfilter_vf_jnd_outputs,
    .process_command = process_command,
};
