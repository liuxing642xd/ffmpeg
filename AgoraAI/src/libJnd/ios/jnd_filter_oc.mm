#import "jnd_filter_oc.h"
#import <Metal/Metal.h>
#import "metal_define.h"

#include "half_float.h"


@implementation JNDFilter

- (instancetype) init
{
  if ((self = [super init])){
    _context = MetalEngine.defaultContext;
    
    kernelCfg[0] = [[MetalKernelCfg alloc] initWithName:@"jnd_conv1" context:_context];
    kernelCfg[1] = [[MetalKernelCfg alloc] initWithName:@"jnd_conv2" context:_context];
    kernelCfg[2] = [[MetalKernelCfg alloc] initWithName:@"jnd_conv3" context:_context];
  }
  isInited = true;
  return self;
}

- (void) unInit
{
  for(int i = 0; i < 3; i++) {
    kernelCfg[i] = nil;
    weights[i] = nil;
    bias[i] = nil;
    size[i] = nil;
  }
  
  for(int i = 0; i < 4; i++) {
    inOutTexture[i] = nil;
  }
  
#if defined(USE_OUTPUT_BUFFER)
  outBuffer = nil;
#endif
  
  isInited = false;
  mWidth = 0;
  mHeight = 0;
  mStride = 0;
}

- (bool) prepare: (int)w height:(int)h stride:(int)stride input:(unsigned char*)input output:(unsigned char*)output
{
  mWidth = w;
  mHeight = h;
  mStride = stride;
  
  for(int i = 0; i < 4; i++) {
    int tmpW = w;
    int tmpH = h;
    MTLPixelFormat pixelFormat = AgoraMetalPixelFormatRGBAFloat;
#if defined(USE_IOSURFACE)
    if(@available(iOS 11.0, *)) {
        if (i == 0 || i == 3) {
            continue;
        }
    } else {
        if (i == 0) {
            tmpW = mStride / 4;
            pixelFormat = MTLPixelFormatRGBA8Unorm;
        }
        if(i == 3) {
            tmpW = mStride / 4;
            pixelFormat = MTLPixelFormatRGBA8Unorm;
        }
    }
#else
    if (i == 0) {
      tmpW = mStride / 4;
      pixelFormat = MTLPixelFormatRGBA8Unorm;// MTLPixelFormatBGRA8Unorm;
    }
#if defined(USE_OUTPUT_BUFFER)
    if(i == 3) {
      continue;
    }
#else
    if(i == 3) {
      tmpW = mStride / 4;
      pixelFormat = MTLPixelFormatRGBA8Unorm;// MTLPixelFormatBGRA8Unorm;
      //tmpW = mStride / 2;
      //pixelFormat = MTLPixelFormatRG8Unorm;
    }
#endif
#endif
    
    MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                          width:tmpW
                                                                                         height:tmpH
                                                                                      mipmapped:YES];
    descriptor.usage = MTLTextureUsageShaderWrite|MTLTextureUsageShaderRead;
    inOutTexture[i] = [_context.device newTextureWithDescriptor:descriptor];
  }
  
  threadsPerThreadGroup[0] = MTLSizeMake(2, 16, 1);
  threadsPerThreadGroup[1] = MTLSizeMake(4, 8, 1);//MTLSizeMake(4, 8, 1);// MTLSizeMake(8, 4, 1);
  threadsPerThreadGroup[2] = MTLSizeMake(4, 8, 1);// MTLSizeMake(8, 4, 1)
  
#if defined(USE_IOSURFACE)
  if(@available(iOS 11.0, *)) {
      NSDictionary* cvBufferProperties = @{ (__bridge NSString*)kCVPixelBufferOpenGLCompatibilityKey : @YES,
                                            (__bridge NSString*)kCVPixelBufferMetalCompatibilityKey : @YES,
                                            };
      CVReturn ret = CVPixelBufferCreate(kCFAllocatorDefault,
                                         mWidth / 4, mHeight,
                                         kCVPixelFormatType_32BGRA,
                                         (__bridge CFDictionaryRef)cvBufferProperties,
                                         &_inputPixelBuf);
      const IOSurfaceRef surface0 = CVPixelBufferGetIOSurface(_inputPixelBuf); // available in ios11 sdk, ios4 runtime
      
      MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                   width:mStride / 4
                                                                                                  height:mHeight
                                                                                               mipmapped:NO];
      textureDescriptor.usage = MTLTextureUsageShaderWrite|MTLTextureUsageShaderRead;
      inOutTexture[0] = [_context.device newTextureWithDescriptor:textureDescriptor iosurface:surface0 plane:0];
      
#if defined(USE_OUTPUT_BUFFER)
      outBuffer = [_context.device newBufferWithLength:mStride*mHeight options:MTLResourceCPUCacheModeDefaultCache|MTLResourceStorageModeShared];
#else
      ret = CVPixelBufferCreate(kCFAllocatorDefault,
                                mWidth / 4, mHeight,
                                kCVPixelFormatType_32BGRA,
                                (__bridge CFDictionaryRef)cvBufferProperties,
                                &_outputPixelBuf);
      const IOSurfaceRef surface1 = CVPixelBufferGetIOSurface(_outputPixelBuf);
      inOutTexture[3] = [_context.device newTextureWithDescriptor:textureDescriptor iosurface:surface1 plane:0];
#endif
  }
#endif
  
  size1[0] = w / 4;
  size1[1] = h;
  
  size2[0] = w;
  size2[1] = h;

  size3[0] = w;
  size3[1] = h;
//  unsigned int tmpSize[2];
//  tmpSize[0] = w / 4;
//  tmpSize[1] = h;
//  size[0] = [_context.device newBufferWithBytes:tmpSize length:8 options:MTLResourceOptionCPUCacheModeWriteCombined];
//  unsigned int tmpSize1[2];
//  tmpSize1[0] = w;
//  tmpSize1[1] = h;
//  size[1] = [_context.device newBufferWithBytes:tmpSize1 length:8 options:MTLResourceOptionCPUCacheModeWriteCombined];
//  unsigned int tmpSize2[2];
//  tmpSize2[0] = w;
//  tmpSize2[1] = h;
//  size[2] = [_context.device newBufferWithBytes:tmpSize2 length:8 options:MTLResourceOptionCPUCacheModeWriteCombined];

  return true;
}

- (bool) process:(unsigned char*)input output:(unsigned char*)output width:(int)w height:(int)h stride:(int)stride
{
  if(!isInited) {
    NSLog(@"Not Initialized! \n");
    return false;
  }
  
  if(mWidth != w || mHeight != h || mStride != stride) {
    [self prepare:w height:h stride:stride input:input output:output];
  }
  
#if defined(USE_IOSURFACE)
  if(@available(iOS 11.0, *)) {
    //lock and copy data to CVPixelBuffer
    CVPixelBufferLockBaseAddress(_inputPixelBuf, 0);
    size_t stridePlane = CVPixelBufferGetBytesPerRowOfPlane(_inputPixelBuf, 0);
    if(stride != stridePlane) {
      NSLog(@"error occured process stride not equal stridePlane!\n");
    }
    uint8_t* dstY = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_inputPixelBuf, 0);
    memcpy(dstY, input, stride*h);
    CVPixelBufferUnlockBaseAddress(_inputPixelBuf, 0);
  } else {
    [inOutTexture[0] replaceRegion:MTLRegionMake2D(0, 0, mStride / 4, h) mipmapLevel:0 withBytes:input bytesPerRow:mStride];
  }
#else
  [inOutTexture[0] replaceRegion:MTLRegionMake2D(0, 0, mStride / 4, h) mipmapLevel:0 withBytes:input bytesPerRow:mStride];
#endif
  
  for(int i = 0; i < 3; i++) {
    [kernelCfg[i] setInTexture:inOutTexture[i]];
    if(i == 2) {
#if defined(USE_OUTPUT_BUFFER)
      [kernelCfg[i] setOutBuffer:outBuffer];
#else
      [kernelCfg[i] setOutTexture:inOutTexture[i+1]];
#endif
    } else {
      [kernelCfg[i] setOutTexture:inOutTexture[i+1]];
    }
//    [kernelCfg[i] setWeights: weights[i]];
//    [kernelCfg[i] setBias:bias[i]];
//    [kernelCfg[i] setSize:size[i]];
    kernelCfg[i]->weights = weights[i];
    kernelCfg[i]->bias = bias[i];
    kernelCfg[i]->size = size[i];
    kernelCfg[i]->weightLength = weightLength[i];
    kernelCfg[i]->biasLength = biasLength[i];
    if(i == 1) {
//      [kernelCfg[i] setThreadgroupMemoryLength0:10*14*4*2];
      [kernelCfg[i] setThreadsPerThreadGroup:threadsPerThreadGroup[i] width:(w+2)/3 height:h];//(w+2)/3
    } else if(i == 2){
#if defined(USE_HALF)
      [kernelCfg[i] setThreadgroupMemoryLength0:10*18*4*2];
#else
      [kernelCfg[i] setThreadgroupMemoryLength0:10*18*4*4];
#endif
      [kernelCfg[i] setThreadsPerThreadGroup:threadsPerThreadGroup[i] width:w/4 height:h];
    } else {
      [kernelCfg[i] setThreadsPerThreadGroup:threadsPerThreadGroup[i] width:w/4 height:h];
    }
  }
  
  [kernelCfg[0] enqueue];
  [kernelCfg[1] enqueue];
  [kernelCfg[2] enqueue];
  
#if defined(USE_MULTI_THREAD)
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [kernelCfg[0] run: false waitComeleted:false];
  });
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
     [kernelCfg[1] run: false waitComeleted:false];
  });
  [kernelCfg[2] run: false waitComeleted:true];
#else
  
  [kernelCfg[0] run: false waitComeleted:false];
  [kernelCfg[1] run: false waitComeleted:false];
#if defined(USE_OUTPUT_BUFFER)
  [kernelCfg[2] run: true waitComeleted:true];
#else
  [kernelCfg[2] run: false waitComeleted:true];
#endif
#endif
  
  
#if defined(USE_OUTPUT_BUFFER)
  unsigned char *byte = (unsigned char*)[outBuffer contents];
  memcpy(output, byte, mStride*mHeight);
#else
#if defined(USE_IOSURFACE)
  if(@available(iOS 11.0, *)) {
    {
      TimeProfiler t("memcpyout");
      double start = clock();
      CVPixelBufferLockBaseAddress(_outputPixelBuf, 0);
      uint8_t* src = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_outputPixelBuf, 0);
      memcpy(output, src, stride*h);
      CVPixelBufferUnlockBaseAddress(_outputPixelBuf, 0);
      double cost_t = clock() - start;
      NSLog(@"cost memcpy cost time = %f", cost_t / CLOCKS_PER_SEC);
    }
  } else {
    [inOutTexture[3] getBytes:output bytesPerRow:mStride fromRegion:MTLRegionMake2D(0, 0, mStride/4, h) mipmapLevel:0];
  }
#else
  [inOutTexture[3] getBytes:output bytesPerRow:mStride fromRegion:MTLRegionMake2D(0, 0, mStride/4, h) mipmapLevel:0];
#endif
#endif
  return true;
}

- (bool) tuning:(unsigned char*)input output:(unsigned char*)output width:(int)w height:(int)h stride:(int)stride
{
  
  if(!isInited) {
      NSLog(@"Not Initialized! \n");
      return false;
    }
    
    if(mWidth != w || mHeight != h || mStride != stride) {
      [self prepare:w height:h stride:stride input:input output:output];
    }
    
  #if defined(USE_IOSURFACE)
    //lock and copy data to CVPixelBuffer
    CVPixelBufferLockBaseAddress(_inputPixelBuf, 0);
    size_t stridePlane = CVPixelBufferGetBytesPerRowOfPlane(_inputPixelBuf, 0);
    if(stride != stridePlane) {
      NSLog(@"error occured process stride not equal stridePlane!\n");
    }
    uint8_t* dstY = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_inputPixelBuf, 0);
    memcpy(dstY, input, stride*h);
    CVPixelBufferUnlockBaseAddress(_inputPixelBuf, 0);
  #else
    [inOutTexture[0] replaceRegion:MTLRegionMake2D(0, 0, mStride / 4, h) mipmapLevel:0 withBytes:input bytesPerRow:mStride];
  #endif
    
    for(int i = 0; i < 3; i++) {
      [kernelCfg[i] setInTexture:inOutTexture[i]];
      if(i == 2) {
  #if defined(USE_OUTPUT_BUFFER)
        [kernelCfg[i] setOutBuffer:outBuffer];
  #else
        [kernelCfg[i] setOutTexture:inOutTexture[i+1]];
  #endif
      } else {
        [kernelCfg[i] setOutTexture:inOutTexture[i+1]];
      }
//      [kernelCfg[i] setWeights: weights[i]];
//      [kernelCfg[i] setBias:bias[i]];
//      [kernelCfg[i] setSize:size[i]];
      kernelCfg[i]->weights = weights[i];
      kernelCfg[i]->bias = bias[i];
      kernelCfg[i]->size = size[i];
      kernelCfg[i]->weightLength = weightLength[i];
      kernelCfg[i]->biasLength = biasLength[i];
      if(i == 1) {
        //[kernelCfg[i] setThreadgroupMemoryLength:10*14*4*2];
        [kernelCfg[i] setThreadsPerThreadGroup:threadsPerThreadGroup[i] width:(w+2)/3 height:h];
      } else if(i == 2){
#if defined(USE_HALF)
        [kernelCfg[i] setThreadgroupMemoryLength0:10*18*4*2];
#else
        [kernelCfg[i] setThreadgroupMemoryLength0:10*18*4*4];
#endif
        [kernelCfg[i] setThreadsPerThreadGroup:threadsPerThreadGroup[i] width:w/4 height:h];
      } else {
        [kernelCfg[i] setThreadsPerThreadGroup:threadsPerThreadGroup[i] width:w/4 height:h];
      }
    }
      
    [kernelCfg[0] enqueue];
    [kernelCfg[0] run:false waitComeleted:false];
    //[kernelCfg[1] enqueue];
    [kernelCfg[1] tuning:false width:w height:h];
    [kernelCfg[2] enqueue];
    [kernelCfg[2] run:false waitComeleted:true];
    
 
  #if defined(USE_OUTPUT_BUFFER)
    unsigned char *byte = (unsigned char*)[outBuffer contents];
    memcpy(output, byte, mStride*mHeight);
  #else
  #if defined(USE_IOSURFACE)
    {
      TimeProfiler t("memcpyout");
      double start = clock();
      CVPixelBufferLockBaseAddress(_outputPixelBuf, 0);
      uint8_t* src = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_outputPixelBuf, 0);
      memcpy(output, src, stride*h);
      CVPixelBufferUnlockBaseAddress(_outputPixelBuf, 0);
      double cost_t = clock() - start;
      NSLog(@"cost memcpy cost time = %f", cost_t / CLOCKS_PER_SEC);
    }
  #else
    [inOutTexture[3] getBytes:output bytesPerRow:mStride fromRegion:MTLRegionMake2D(0, 0, mStride/4, h) mipmapLevel:0];
  #endif
  #endif
  return true;
}

- (bool) update_net_paras:(JND_NET_PARAS)net_paras
{
#if defined(USE_HALF)
  Float32 temp_weight1[36];
  Float32 temp_weight2[144];
  Float32 temp_weight3[36];
  Float32 temp_bias1[4];
  Float32 temp_bias2[4];
  Float32 temp_bias3[4];
  
  nchw_2_hwcn(net_paras.conv1_weight, temp_weight1, 4, 3, 3, 1);
  memcpy(temp_bias1, net_paras.conv1_bias, 4*sizeof(Float32));
  nchw_2_hwcn(net_paras.conv2_weight, temp_weight2, 4, 3, 3, 4);
  //nchw_2_nhwc(net_paras.conv2_weight, temp_weight2, 4, 3, 3, 4);
  //nchw_2_hwnc(net_paras.conv2_weight, temp_weight2, 4, 3, 3, 4);
  memcpy(temp_bias2, net_paras.conv2_bias, 4*sizeof(Float32));
  nchw_2_hwcn(net_paras.conv3_weight, temp_weight3, 1, 3, 3, 4);
  
  for(int i = 0; i < 4; i++) {
    temp_bias3[i] = net_paras.conv3_bias[0];
  }
  
  //convert to half
//  metal_half temp_half_weight1[36];
//  metal_half temp_half_weight2[144];
//  metal_half temp_half_weight3[36];
//
//  metal_half temp_half_bias1[4];
//  metal_half temp_half_bias2[4];
//  metal_half temp_half_bias3[4];
  
  conv_float_2_half_array(temp_weight1, temp_half_weight1, 36);
  conv_float_2_half_array(temp_weight2, temp_half_weight2, 144);
  conv_float_2_half_array(temp_weight3, temp_half_weight3, 36);
  conv_float_2_half_array(temp_bias1, temp_half_bias1, 4);
  conv_float_2_half_array(temp_bias2, temp_half_bias2, 4);
  conv_float_2_half_array(temp_bias3, temp_half_bias3, 4);

//  weights[0] = [_context.device newBufferWithBytes:temp_half_weight1 length:36*2 options:MTLResourceOptionCPUCacheModeWriteCombined];
//  weights[1] = [_context.device newBufferWithBytes:temp_half_weight2 length:144*2 options:MTLResourceOptionCPUCacheModeWriteCombined];
//  weights[2] = [_context.device newBufferWithBytes:temp_half_weight3 length:36*2 options:MTLResourceOptionCPUCacheModeWriteCombined];
//
//  bias[0] = [_context.device newBufferWithBytes:temp_half_bias1 length:8 options:MTLResourceOptionCPUCacheModeWriteCombined];
//  bias[1] = [_context.device newBufferWithBytes:temp_half_bias2 length:8 options:MTLResourceOptionCPUCacheModeWriteCombined];
//  bias[2] = [_context.device newBufferWithBytes:temp_half_bias3 length:8 options:MTLResourceOptionCPUCacheModeWriteCombined];
  
  weights[0] = temp_half_weight1;
  weights[1] = temp_half_weight2;
  weights[2] = temp_half_weight3;
  weightLength[0] = 36*2;
  weightLength[1] = 144*2;
  weightLength[2] = 36*2;
  
  bias[0] = temp_half_bias1;
  bias[1] = temp_half_bias2;
  bias[2] = temp_half_bias3;
  biasLength[0] = 4*2;
  biasLength[1] = 4*2;
  biasLength[2] = 4*2;

  
  size[0] = size1;
  size[1] = size2;
  size[2] = size3;
#else
  Float32 temp_weight1[36];
  Float32 temp_weight2[144];
  Float32 temp_weight3[36];
  Float32 temp_bias1[4];
  Float32 temp_bias2[4];
  Float32 temp_bias3[4];
  
  nchw_2_hwcn(net_paras.conv1_weight, temp_weight1, 4, 3, 3, 1);
  memcpy(temp_bias1, net_paras.conv1_bias, 4*sizeof(Float32));
  nchw_2_hwcn(net_paras.conv2_weight, temp_weight2, 4, 3, 3, 4);
  memcpy(temp_bias2, net_paras.conv2_bias, 4*sizeof(Float32));
  nchw_2_hwcn(net_paras.conv3_weight, temp_weight3, 1, 3, 3, 4);
  
  for(int i = 0; i < 4; i++) {
    temp_bias3[i] = net_paras.conv3_bias[0];
  }
  
  weights[0] = temp_half_weight1;
  weights[1] = temp_half_weight2;
  weights[2] = temp_half_weight3;
  weightLength[0] = 36*4;
  weightLength[1] = 144*4;
  weightLength[2] = 36*4;
  
  bias[0] = temp_half_bias1;
  bias[1] = temp_half_bias2;
  bias[2] = temp_half_bias3;
  biasLength[0] = 4*4;
  biasLength[1] = 4*4;
  biasLength[2] = 4*4;
  size[0] = size1;
  size[1] = size2;
  size[2] = size3;
#endif
  return true;
}

@end
