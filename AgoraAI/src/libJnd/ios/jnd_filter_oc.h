#import <Foundation/Foundation.h>
#import "metal_engine.h"
#import "metal_kernel_cfg.h"

#import "metal_define.h"
#if defined(USE_IOSURFACE)
#import <CoreVideo/CVPixelBuffer.h>
#import <CoreVideo/CVMetalTexture.h>
#import <CoreVideo/CVMetalTextureCache.h>
#endif

#include "jnd_filter_metal.h"

@interface JNDFilter : NSObject {
  @private
  MetalKernelCfg* kernelCfg[3];
  id<MTLTexture> inOutTexture[4];
#if defined(USE_OUTPUT_BUFFER)
  id<MTLBuffer> outBuffer;
#endif
  
//  id<MTLBuffer> weights[3];
//  id<MTLBuffer> bias[3];
//  id<MTLBuffer> size[3];
  const void* weights[3];
  const void* bias[3];
  const void* size[3];
  int weightLength[3];
  int biasLength[3];
  MTLSize threadsPerThreadGroup[3];
  bool isInited;
  int mWidth;
  int mHeight;
  int mStride;
  
  @public
  int size1[2];
  int size2[2];
  int size3[2];
  
  metal_half temp_half_weight1[36];
  metal_half temp_half_weight2[144];
  metal_half temp_half_weight3[36];

  metal_half temp_half_bias1[4];
  metal_half temp_half_bias2[4];
  metal_half temp_half_bias3[4];
}

@property (nonatomic, strong) MetalEngine *context;

#if defined(USE_IOSURFACE)
@property (nonatomic, assign) CVPixelBufferRef inputPixelBuf;
@property (nonatomic, assign) CVPixelBufferRef outputPixelBuf;
#endif

- (instancetype) init;

- (void) unInit;

- (bool) prepare: (int)w height:(int)h stride:(int)stride input:(unsigned char*)input output:(unsigned char*)output;

- (bool) process:(unsigned char*)input output:(unsigned char*)output width:(int)w height:(int)h stride:(int)stride;

- (bool) tuning:(unsigned char*)input output:(unsigned char*)output width:(int)w height:(int)h stride:(int)stride;

- (bool) update_net_paras:(JND_NET_PARAS)net_paras;

@end
