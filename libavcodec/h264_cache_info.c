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
 * Store h264 decoded info which will be used in transcoding step.
 * @author Dany Xue <xueduan@agora.io>
 */

#include "libavutil/error.h"
#include "h264_cache_info.h"


int frame_cache_info_init(FrameCachedInfoPool *cache)
{
    int i;

    if (!cache)
        return AVERROR(EINVAL);
    
    if (cache->pool) {
        av_buffer_pool_uninit(&cache->pool);
    }

    cache->pool =  av_buffer_pool_init(sizeof(FrameCachedInfo), av_buffer_allocz);
    if (!cache->pool)
        goto fail;

    for(i = 0; i < MAX_SIDE_DATA_CACHE_SIZE; i++) {
        cache->bufs[i] = av_buffer_pool_get(cache->pool);
        if (!cache->bufs[i])
            goto fail;
    }

    return 0;

fail:
    av_buffer_pool_uninit(&cache->pool);
    for(i = 0; i < MAX_SIDE_DATA_CACHE_SIZE; i++) {
        if (cache->bufs[i])
            av_buffer_unref(&cache->bufs[i]);
    }

    return AVERROR(EINVAL);
}

void frame_cache_info_uninit(FrameCachedInfoPool *cache)
{
    int i;

    if (!cache)
        return;

    for(i = 0; i < MAX_SIDE_DATA_CACHE_SIZE; i++) {
        if (cache->bufs[i])
            av_buffer_unref(&cache->bufs[i]);
    }

    if(cache->pool)
        av_buffer_pool_uninit(&cache->pool);
}


AVBufferRef *request_buffer_from_cache(FrameCachedInfoPool *cache)
{
    FrameCachedInfo *info = NULL;
    AVBufferRef *buf = NULL;
    int i;

    if (!cache || !cache->pool)
        return NULL;

    for(int i = 0; cache->bufs[i] && i < MAX_SIDE_DATA_CACHE_SIZE; i++) {
        info = (FrameCachedInfo*) cache->bufs[i]->data;
        if (info && !info->used) {
            buf = cache->bufs[i];
            break;
        }
    }

    return av_buffer_ref(buf);
}

