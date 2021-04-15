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

#define MAX_SIDE_DATA_CACHE_SIZE 10
/**
 * Structure describing decoded info of one video frame
 */
typedef struct FrameCachedInfo {

    int64_t rsv0;
    uint32_t rsv1;
    uint32_t rsv2;
    uint32_t rsv3;
    uint32_t rsv4;
    uint32_t rsv5;

    uint32_t used;

} FrameCachedInfo;

typedef struct FrameCachedInfoPool{
    AVBufferRef *bufs[MAX_SIDE_DATA_CACHE_SIZE];
    AVBufferPool *pool;
} FrameCachedInfoPool;


int frame_cache_info_init(FrameCachedInfoPool *cache_pool);
void frame_cache_info_uninit(FrameCachedInfoPool *cache_pool);

AVBufferRef *request_buffer_from_cache(FrameCachedInfoPool *cache_pool);


#endif /* AVCODEC_H264CACHEINFO_H */
