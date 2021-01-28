#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#import "metal_engine.h"


@interface MetalKernelCfg : NSObject {
  @public
  const void* weights;
  const void* bias;
  const void* size;
  int weightLength;
  int biasLength;
}

@property (nonatomic, strong) MetalEngine* context;
@property (nonatomic, strong) id<MTLFunction> kernelFunc;
@property (nonatomic, strong) id<MTLComputePipelineState> pipeline;
//@property (nonatomic, strong) NSString* kernelName;
@property (nonatomic, strong) id<MTLTexture> inTexture;
@property (nonatomic, strong) id<MTLTexture> outTexture;
@property (nonatomic, strong) id<MTLBuffer> outBuffer;
//@property (nonatomic, strong) id<MTLBuffer> weights;
//@property (nonatomic, strong) id<MTLBuffer> bias;
//@property (nonatomic, strong) id<MTLBuffer> size;

@property (nonatomic, assign) MTLSize threadsPerThreadGroup;
@property (nonatomic, assign) MTLSize threadGroups;
@property (nonatomic, assign) NSUInteger threadgroupMemoryLength0;

@property (nonatomic, strong) id<MTLCommandBuffer> commandBuffer;

- (instancetype) initWithName:(NSString*)funcName context:(MetalEngine*)context;

- (void) setThreadsPerThreadGroup:(MTLSize)threadgroupCount width:(int)w height:(int)h;

- (void) enqueue;

- (bool) run: (bool)outToBuf waitComeleted:(bool)waitCompleted;

- (bool) tuning: (bool)outToBuf width:(int)w height:(int)h;

@end
