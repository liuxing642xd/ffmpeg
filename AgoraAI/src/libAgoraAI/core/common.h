#pragma once

#ifdef _WIN32
#define ALIGN_START __declspec(align(128))
#define ALIGN_END
#else
#define ALIGN_START 
#define ALIGN_END __attribute__((aligned(128)))
#endif

#ifdef _WIN32
#define PLATFORM_X86
#include "NEON_2_SSE.h"
#define USE_SSE
#define USE_AVX
#define USE_NEON
#elif __unix__
#define USE_AVX
#define PLATFORM_X86
//add something here for linux
#elif __APPLE__
//add something here for mac
#else
#define PLATFORM_ARM
#include <arm_neon.h>
#define USE_NEON
#endif

#define MAX2(x, y) ((x) > (y) ? (x) : (y))
#define MIN2(x, y) ((x) > (y) ? (y) : (x))

typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
