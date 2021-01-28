//
//  JNDKernels.metal
//  ImageProcessing
//
//  Created by 陈飞飞 on 2019/12/17.
//  Copyright © 2019 Metal By Example. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

#define HALF_FP

#ifdef HALF_FP
#define DATA_TYPE half
#define DATA_TYPE2 half2
#define DATA_TYPE3 half3
#define DATA_TYPE4 half4
#else
//#define DATA_TYPE float
//#define DATA_TYPE2 float2
//#define DATA_TYPE3 float3
//#define DATA_TYPE4 float4
#endif

kernel void jnd_conv1(texture2d<DATA_TYPE, access::read> inTexture [[texture(0)]],
                      texture2d<DATA_TYPE, access::write> outTexture [[texture(1)]],
                      constant DATA_TYPE4* weight [[buffer(0)]],
                      constant DATA_TYPE4* biass [[buffer(1)]],
                      ushort2 gid [[thread_position_in_grid]])
{
  ushort w = gid.x;
  ushort h = gid.y;

  if(h >= inTexture.get_height() || w >= inTexture.get_width())
    return;
    
  DATA_TYPE4 output[4];
  output[0] = biass[0];
  output[1] = output[0];
  output[2] = output[0];
  output[3] = output[0];
  
  DATA_TYPE4 input[3];
  
  for(int i = 0; i < 3; i++) {
    ushort width0 = clamp(w-1, 0, (ushort)inTexture.get_width()-1);
    ushort width1 = clamp(w+1, 0, (ushort)inTexture.get_width()-1);
    ushort height = clamp(h-1+i, 0, (ushort)inTexture.get_height()-1);
    input[0] = inTexture.read(uint2(width0, height));
    input[1] = inTexture.read(uint2(w, height));
    input[2] = inTexture.read(uint2(width1, height));
    output[0] += input[0].w * weight[3*i];
    output[0] += input[1].x * weight[3*i+1];
    output[0] += input[1].y * weight[3*i+2];
    output[1] += input[1].x * weight[3*i];
    output[1] += input[1].y * weight[3*i+1];
    output[1] += input[1].z * weight[3*i+2];
    output[2] += input[1].y * weight[3*i];
    output[2] += input[1].z * weight[3*i+1];
    output[2] += input[1].w * weight[3*i+2];
    output[3] += input[1].z * weight[3*i];
    output[3] += input[1].w * weight[3*i+1];
    output[3] += input[2].x * weight[3*i+2];
  }
  
  output[0] = fmax(output[0], 0.0h);
  output[1] = fmax(output[1], 0.0h);
  output[2] = fmax(output[2], 0.0h);
  output[3] = fmax(output[3], 0.0h);
  outTexture.write(output[0], uint2(w*4, h));
  outTexture.write(output[1], uint2(w*4+1, h));
  outTexture.write(output[2], uint2(w*4+2, h));
  outTexture.write(output[3], uint2(w*4+3, h));
}


#define CONV2_CORE(w0, w1, w2, w3, in0, in1, in2, out0, out1, out2) \
out0 += w0 * in0.x; \
out0 += w1 * in0.y; \
out0 += w2 * in0.z; \
out0 += w3 * in0.w; \
out1 += w0 * in1.x; \
out1 += w1 * in1.y; \
out1 += w2 * in1.z; \
out1 += w3 * in1.w; \
out2 += w0 * in2.x; \
out2 += w1 * in2.y; \
out2 += w2 * in2.z; \
out2 += w3 * in2.w;

kernel void jnd_conv2(texture2d<DATA_TYPE, access::read> inTexture [[texture(0)]],
                      texture2d<DATA_TYPE, access::write> outTexture [[texture(1)]],
                      constant DATA_TYPE4* wt [[buffer(0)]],
                      constant DATA_TYPE4* biass [[buffer(1)]],
                      constant uint2* wh [[buffer(2)]],
                      ushort2 gid [[thread_position_in_grid]])
{
  ushort x = gid.x * 3;
  ushort y = gid.y;

  if(x >= inTexture.get_width() || y >= inTexture.get_height())
    return;
  
  DATA_TYPE4 out[3];
  out[0] = biass[0];
  out[1] = out[0];
  out[2] = out[0];
  
  int offset = 0;
  for(int i = 0; i < 3; i++) {
     DATA_TYPE4 in[5];
     ushort width0 = clamp(x-1, 0, (ushort)inTexture.get_width()-1);
     ushort width1 = clamp(x+0, 0, (ushort)inTexture.get_width()-1);
     ushort width2 = clamp(x+1, 0, (ushort)inTexture.get_width()-1);
     ushort width3 = clamp(x+2, 0, (ushort)inTexture.get_width()-1);
     ushort width4 = clamp(x+3, 0, (ushort)inTexture.get_width()-1);
     ushort pos_y = clamp(y+i-1, 0, (ushort)inTexture.get_height()-1);
     in[0] = inTexture.read(uint2(width0, pos_y));
     in[1] = inTexture.read(uint2(width1, pos_y));
     in[2] = inTexture.read(uint2(width2, pos_y));
     in[3] = inTexture.read(uint2(width3, pos_y));
     in[4] = inTexture.read(uint2(width4, pos_y));
     
     CONV2_CORE(wt[offset+0], wt[offset+1], wt[offset+2], wt[offset+3], in[0], in[1], in[2], out[0], out[1], out[2]);

     CONV2_CORE(wt[offset+4], wt[offset+5], wt[offset+6], wt[offset+7], in[1], in[2], in[3], out[0], out[1], out[2]);

     CONV2_CORE(wt[offset+8], wt[offset+9], wt[offset+10], wt[offset+11], in[2], in[3], in[4], out[0], out[1], out[2]);
     offset += 12;
   }
   
   out[0] = fmax(out[0], DATA_TYPE4(0.0h));
   out[1] = fmax(out[1], DATA_TYPE4(0.0h));
   out[2] = fmax(out[2], DATA_TYPE4(0.0h));

   outTexture.write(out[0], uint2(x, y));
   outTexture.write(out[1], uint2(x+1, y));
   outTexture.write(out[2], uint2(x+2, y));
}


kernel void jnd_conv3(texture2d<DATA_TYPE, access::read> inTexture [[texture(0)]],
                      texture2d<DATA_TYPE, access::write> outTexture [[texture(1)]],
                      constant DATA_TYPE4* weight [[buffer(0)]],
                      constant DATA_TYPE4* biass [[buffer(1)]],
                      constant uint2* wh [[buffer(2)]],
                      threadgroup DATA_TYPE4 *data [[threadgroup(0)]],
                      ushort2 threads [[thread_position_in_threadgroup]],//threads_per_threadgroup
                      ushort tid [[thread_index_in_threadgroup]],
                      ushort2 gid [[thread_position_in_grid]],
                      ushort2 tgid [[threadgroup_position_in_grid]])
{
  ushort grid_x = tgid.x;
  ushort grid_y = tgid.y;
    
  if(tid < 30) {
    ushort idx_y = tid / 6; //0-4
    ushort idx_x = tid % 6; //0-5
    DATA_TYPE4 input[6];
    short posx = grid_x * 16 + idx_x * 3 - 1;
    short posx0 = clamp(short(posx+0), short(0), (short)(inTexture.get_width()-1));
    short posx1 = clamp(short(posx+1), short(0), (short)(inTexture.get_width()-1));
    short posx2 = clamp(short(posx+2), short(0), (short)(inTexture.get_width()-1));
    short posy0 = clamp(grid_y * 8 + idx_y * 2 - 1, 0, (short)inTexture.get_height()-1);
    short posy1 = clamp(grid_y * 8 + idx_y * 2, 0, (short)inTexture.get_height()-1);
    input[0] = inTexture.read(uint2(posx0, posy0));
    input[1] = inTexture.read(uint2(posx1, posy0));
    input[2] = inTexture.read(uint2(posx2, posy0));
    input[3] = inTexture.read(uint2(posx0, posy1));
    input[4] = inTexture.read(uint2(posx1, posy1));
    input[5] = inTexture.read(uint2(posx2, posy1));
    short sm_idx = idx_y * 36 + idx_x * 3;
    data[sm_idx] = input[0];
    data[sm_idx + 1] = input[1];
    data[sm_idx + 2] = input[2];
    data[sm_idx + 18] = input[3];
    data[sm_idx + 19] = input[4];
    data[sm_idx + 20] = input[5];
  }
  threadgroup_barrier(mem_flags::mem_threadgroup);

  if(gid.x >= outTexture.get_width() || gid.y >= outTexture.get_height())
    return;
  
  DATA_TYPE4 output[4];
  output[0] = DATA_TYPE4(0.0f);
  output[1] = output[0];
  output[2] = output[0];
  output[3] = output[0];
  for(int i = 0 ; i < 3; i++) {
    DATA_TYPE4 input[6];
    short index = (threads.y+i)*18+threads.x*4;
    input[0] = data[index];
    input[1] = data[index+1];
    input[2] = data[index+2];
    input[3] = data[index+3];
    input[4] = data[index+4];
    input[5] = data[index+5];
    output[0] += input[0] * weight[3*i];
    output[0] += input[1] * weight[3*i+1];
    output[0] += input[2] * weight[3*i+2];
    output[1] += input[1] * weight[3*i];
    output[1] += input[2] * weight[3*i+1];
    output[1] += input[3] * weight[3*i+2];
    output[2] += input[2] * weight[3*i];
    output[2] += input[3] * weight[3*i+1];
    output[2] += input[4] * weight[3*i+2];
    output[3] += input[3] * weight[3*i];
    output[3] += input[4] * weight[3*i+1];
    output[3] += input[5] * weight[3*i+2];
  }
  
  DATA_TYPE4 fout = DATA_TYPE4(dot(output[0], DATA_TYPE4(1.0h)),
                       dot(output[1], DATA_TYPE4(1.0h)),
                       dot(output[2], DATA_TYPE4(1.0h)),
                       dot(output[3], DATA_TYPE4(1.0h))) + biass[0]+DATA_TYPE4(0.0019608h);
  
  outTexture.write(fout, uint2(gid));
}

/*
kernel void jnd_conv3_new(texture2d<DATA_TYPE, access::read> inTexture [[texture(0)]],
                      texture2d<DATA_TYPE, access::write> outTexture [[texture(1)]],
                      constant DATA_TYPE4 *weight [[buffer(0)]],
                      constant DATA_TYPE4 *biass [[buffer(1)]],
                      ushort2 gid [[thread_position_in_grid]])
{
  ushort x = gid.x;
  ushort h = gid.y;
  
  ushort w = x * 4;
  
  if(gid.x >= outTexture.get_width() || gid.y >= outTexture.get_height())
    return;
  
  DATA_TYPE4 output[4];
  output[0] = DATA_TYPE4(0.0f);
  output[1] = DATA_TYPE4(0.0f);
  output[2] = DATA_TYPE4(0.0f);
  output[3] = DATA_TYPE4(0.0f);
  
  for(int i = 0; i < 3; i++) {
    DATA_TYPE4 input[3];
    ushort posx0 = clamp(w-1, 0, (ushort)inTexture.get_width()-1);
    ushort posx1 = clamp(w+1, 0, (ushort)inTexture.get_width()-1);
    ushort posx2 = clamp(w+2, 0, (ushort)inTexture.get_width()-1);
    ushort posx3 = clamp(w+3, 0, (ushort)inTexture.get_width()-1);
    ushort posx4 = clamp(w+4, 0, (ushort)inTexture.get_width()-1);
    ushort posy = clamp(h-1+i, 0, (ushort)inTexture.get_height()-1);
    input[0] = inTexture.read(uint2(posx0, posy));
    input[1] = inTexture.read(uint2(w, posy));
    input[2] = inTexture.read(uint2(posx1, posy));
    
    output[0] += input[0] * weight[3*i];
    output[0] += input[1] * weight[3*i+1];
    output[0] += input[2] * weight[3*i+2];
    
    input[0] = inTexture.read(uint2(posx2, posy));
    output[1] += input[1] * weight[3*i];
    output[1] += input[2] * weight[3*i+1];
    output[1] += input[0] * weight[3*i+2];
    
    input[1] = inTexture.read(uint2(posx3, posy));
    output[2] += input[2] * weight[3*i];
    output[2] += input[0] * weight[3*i+1];
    output[2] += input[1] * weight[3*i+2];
    
    input[2] = inTexture.read(uint2(posx4, posy));
    output[3] += input[0] * weight[3*i];
    output[3] += input[1] * weight[3*i+1];
    output[3] += input[2] * weight[3*i+2];
  }
  
  DATA_TYPE4 fout = DATA_TYPE4(dot(output[0], DATA_TYPE4(1.0f)),
                       dot(output[1], DATA_TYPE4(1.0f)),
                       dot(output[2], DATA_TYPE4(1.0f)),
                       dot(output[3], DATA_TYPE4(1.0f))) + biass[0];
  
  outTexture.write(fout, uint2(x, h));
}*/
