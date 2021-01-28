//--------------------------------------------------------------------------------------
// File: half_float.h
// Desc:
//
// Author:      QUALCOMM
//
//               Copyright (c) 2018 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#ifndef SDK_EXAMPLES_HALF_FLOAT_H
#define SDK_EXAMPLES_HALF_FLOAT_H

#include <stdint.h>

typedef uint16_t metal_half;
typedef float metal_float;

/**
 * \brief Given a 32-bit float, converts it (potentially with some error due to loss of precision)
 * to a 16-bit half float for use with OpenCL.
 *
 * @param f [in] - The 32-bit float to convert
 * @return the equivalent 16-bit half float
 */

metal_half to_half(metal_float f);

/**
 * \brief Given a 16-bit half float, converts it to a 32-bit float.
 *
 * @param f [in] - The 16-bit half float to convert
 * @return the equivalent 32-bit float
 */
metal_float to_float(metal_half f);

#endif //SDK_EXAMPLES_HALF_FLOAT_H
