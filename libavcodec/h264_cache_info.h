/*
 * Copyright (c) 2021 Dany Xue <xueduan@agora.io>
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
 * Store h264 decoded info which will be used in transcoding stop. 
 * @author Dany Xue <xueduan@agora.io>
 */

#ifndef AVCODEC_H264CACHEINFO_H
#define AVCODEC_H264CACHEINFO_H

#include <stddef.h>
#include <stdint.h>

#include "libavutil/buffer.h"
#include "libavutil/frame.h"

#define MAX_SIDE_DATA_CACHE_SIZE 12

enum X264BlkPartitionEnum
{
    /* sub partition type for P_8x8 and B_8x8 */
    D_L0_4x4          = 0,
    D_L0_8x4          = 1,
    D_L0_4x8          = 2,
    D_L0_8x8          = 3,

    /* sub partition type for B_8x8 only */
    D_L1_4x4          = 4,
    D_L1_8x4          = 5,
    D_L1_4x8          = 6,
    D_L1_8x8          = 7,

    D_BI_4x4          = 8,
    D_BI_8x4          = 9,
    D_BI_4x8          = 10,
    D_BI_8x8          = 11,
    D_DIRECT_8x8      = 12,

    /* partition */
    D_8x8             = 13,
    D_16x8            = 14,
    D_8x16            = 15,
    D_16x16           = 16,
    X264_PARTTYPE_MAX = 17,
};

enum X264MBTypeEnum
{
    I_4x4           = 0,
    I_8x8           = 1,
    I_16x16         = 2,
    I_PCM           = 3,

    P_L0            = 4,
    P_8x8           = 5,
    P_SKIP          = 6,

    B_DIRECT        = 7,
    B_L0_L0         = 8,
    B_L0_L1         = 9,
    B_L0_BI         = 10,
    B_L1_L0         = 11,
    B_L1_L1         = 12,
    B_L1_BI         = 13,
    B_BI_L0         = 14,
    B_BI_L1         = 15,
    B_BI_BI         = 16,
    B_8x8           = 17,
    B_SKIP          = 18,

    X264_MBTYPE_MAX = 19
};

enum X264Intra16x16PredEnum
{
    I_PRED_16x16_V  = 0,
    I_PRED_16x16_H  = 1,
    I_PRED_16x16_DC = 2,
    I_PRED_16x16_P  = 3,

    I_PRED_16x16_DC_LEFT = 4,
    I_PRED_16x16_DC_TOP  = 5,
    I_PRED_16x16_DC_128  = 6,
};

enum X264Intra4x4PredEnum
{
    I_PRED_4x4_V  = 0,
    I_PRED_4x4_H  = 1,
    I_PRED_4x4_DC = 2,
    I_PRED_4x4_DDL= 3,
    I_PRED_4x4_DDR= 4,
    I_PRED_4x4_VR = 5,
    I_PRED_4x4_HD = 6,
    I_PRED_4x4_VL = 7,
    I_PRED_4x4_HU = 8,

    I_PRED_4x4_DC_LEFT = 9,
    I_PRED_4x4_DC_TOP  = 10,
    I_PRED_4x4_DC_128  = 11,
};

/**
 * Structure describing decoded info of one video frame
 */
typedef struct FrameCachedInfo {
    uint32_t used;
    uint32_t slice_type;

    int8_t *mb_type; //size: mb_size
    int8_t *mb_partition; //size: mb_size
    int8_t *luma_intra_pred_mode; //size: width * height /16
    int8_t *chroma_intra_pred_mode; //size mb_size
    int8_t *ref_cache; //size mb_size * 16
    int16_t *mv_cache; //size mb_size * 16 *2
    int16_t *dct_coff_0; //size mb_size * 16
} FrameCachedInfo;

typedef struct FrameCachedInfoPool{
    AVBufferRef *bufs[MAX_SIDE_DATA_CACHE_SIZE];
    AVBufferPool *pool;
    uint32_t buf_size;
} FrameCachedInfoPool;


int frame_cache_info_init(FrameCachedInfoPool *cache_pool, int32_t mb_num);
void frame_cache_info_uninit(FrameCachedInfoPool *cache_pool);

AVBufferRef *request_buffer_from_cache(FrameCachedInfoPool *cache_pool);


#endif /* AVCODEC_H264CACHEINFO_H */
