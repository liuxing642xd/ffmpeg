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
/**
 * Structure describing decoded info of one video frame
 */
typedef struct FrameCachedInfo {
    uint32_t used;
    uint32_t slice_type;

    uint32_t *mb_type; //size: mb_size
    int8_t *luma_intra_pred_mode; //size: width * height /16
    int8_t *chroma_intra_pred_mode; //size mb_size
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
