//
//  metal_define.h
//  libJnd
//
//  Created by 陈飞飞 on 2019/12/24.
//  Copyright © 2019 陈飞飞. All rights reserved.
//

#ifndef metal_define_h
#define metal_define_h

#import <Metal/Metal.h>
#import <float.h>

#include "half_float.h"
#include <chrono>
#include <iostream>

//#define USE_OUTPUT_BUFFER
#if TARGET_OS_SIMULATOR
//ios simulator not support iosurface
#else
//#define USE_IOSURFACE //ios surface ios 11.0
#endif

#define AGORA_METAL_FULL_PRECISION 0
#define USE_HALF
#if AGORA_METAL_FULL_PRECISION
typedef float FLOAT;
#define AgoraMetalPixelFormatRGBAFloat MTLPixelFormatRGBA32Float
#else
typedef __fp16 FLOAT;
#define AgoraMetalPixelFormatRGBAFloat MTLPixelFormatRGBA16Float
#endif


class TimeProfiler
{
public:
  TimeProfiler(std::string name)
  {
    _name = name;

    _start = std::chrono::high_resolution_clock::now();
  }

  ~TimeProfiler()
  {
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float, std::ratio<1, 1000>> duration =
      std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1000>>>(end - _start);

    std::cout << _name << " cost:   " << duration.count() << "ms" << std::endl;
  }

private:
  std::string _name;
  std::chrono::high_resolution_clock::time_point _start;
};

inline void nchw_2_nhwc(float *in, float *out, int n, int h, int w, int c)
{
  for (int i_n = 0; i_n < n; i_n++)
  {
    for (int i_h = 0; i_h < h; i_h++)
    {
      for (int i_w = 0; i_w < w; i_w++)
      {
        for (int i_c = 0; i_c < c; i_c++)
        {
          *out++ = in[(i_n*c + i_c)*h*w + i_h*w + i_w];
        }
      }
    }
  }
}

inline void nchw_2_hwcn(float *in, float *out, int n, int h, int w, int c)
{
  for (int i_h = 0; i_h < h; i_h++)
  {
    for (int i_w = 0; i_w < w; i_w++)
    {
      for (int i_c = 0; i_c < c; i_c++)
      {
        for (int i_n = 0; i_n < n; i_n++)
        {
          *out++ = in[(i_n*c + i_c)*h*w + i_h*w + i_w];
        }
      }
    }
  }
}

inline void nchw_2_hwnc(float *in, float *out, int n, int h, int w, int c)
{
  for (int i_h = 0; i_h < h; i_h++)
  {
    for (int i_w = 0; i_w < w; i_w++)
    {
      for (int i_n = 0; i_n < n; i_n++)
      {
        for (int i_c = 0; i_c < c; i_c++)
        {
          *out++ = in[(i_n*c + i_c)*h*w + i_h*w + i_w];
        }
      }
    }
  }
}

inline void nhwc_2_hwcn(float *in, float *out, int n, int h, int w, int c)
{
  for (int i_h = 0; i_h < h; i_h++)
  {
    for (int i_w = 0; i_w < w; i_w++)
    {
      for (int i_c = 0; i_c < c; i_c++)
      {
        for (int i_n = 0; i_n < n; i_n++)
        {
          *out++ = in[((i_n*h +i_h)*w +i_w)*c + i_c];
        }
      }
    }
  }
}

static void conv_float_2_half_array(float *in, metal_half *out, int num)
{
  for (int i = 0; i < num; i++)
  {
    out[i] = to_half(in[i]);
  }
}


inline double t(double last, char* key){
    clock_t now = clock();
    printf("time:%fs \t key:%s \n", (last != 0) ? (double)(now - last) / CLOCKS_PER_SEC : 0, key);
    return now;
}

#endif /* metal_define_h */
