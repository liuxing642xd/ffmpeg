#import "metal_kernel_cfg.h"

@implementation MetalKernelCfg

- (instancetype) initWithName:(NSString*)funcName context:(MetalEngine*)context
{
  if ((self = [super init])){
    _context = context;
    NSError *error = nil;
    _kernelFunc = [_context.library newFunctionWithName:funcName];
    //_pipeline = [_context.device newComputePipelineStateWithFunction:_kernelFunc error:&error];
    MTLComputePipelineDescriptor* descriptor = [MTLComputePipelineDescriptor new];
    descriptor.computeFunction = _kernelFunc;
    descriptor.threadGroupSizeIsMultipleOfThreadExecutionWidth = YES;
    _pipeline = [_context.device newComputePipelineStateWithDescriptor:descriptor options:MTLPipelineOptionNone reflection:nil error:&error];
    if (!_pipeline)
    {
      NSLog(@"Error occurred when building compute pipeline for function %@", funcName);
      return nil;
    }
    _threadgroupMemoryLength0 = 0;
    NSUInteger threadExecutionWidth = _pipeline.threadExecutionWidth;
    NSUInteger maxTotalThreadPreThreadGroup = _pipeline.maxTotalThreadsPerThreadgroup;
    NSLog(@"threadExecutionWidth %d maxTotalThreadPreThreadGroup %d", threadExecutionWidth, maxTotalThreadPreThreadGroup);
  }
  return self;
}

- (void) setThreadsPerThreadGroup:(MTLSize)threadgroupCount width:(int)w height:(int)h
{
  _threadsPerThreadGroup = threadgroupCount;
  
  if(_inTexture != nil) {
    _threadGroups = MTLSizeMake((w + _threadsPerThreadGroup.width - 1) / _threadsPerThreadGroup.width,
                              (h + _threadsPerThreadGroup.height -1) / _threadsPerThreadGroup.height,
                              1);
  } else {
    NSLog(@"error shoukd set texture first! \n");
  }
}

- (void) enqueue
{
  _commandBuffer = [_context.commandQueue commandBuffer];
  [_commandBuffer enqueue];
}


- (bool) run: (bool)outToBuf waitComeleted:(bool)waitCompleted
{
  id<MTLComputeCommandEncoder> commandEncoder = [_commandBuffer computeCommandEncoder];
  [commandEncoder setComputePipelineState:_pipeline];
  [commandEncoder setTexture:_inTexture atIndex:0];
  if(!outToBuf) {
    [commandEncoder setTexture:_outTexture atIndex:1];
  }
//  [commandEncoder setBuffer:_weights offset:0 atIndex:0];
//  [commandEncoder setBuffer:_bias offset:0 atIndex:1];
//  [commandEncoder setBuffer:_size offset:0 atIndex:2];
  
  [commandEncoder setBytes:weights length:weightLength atIndex:0];
  [commandEncoder setBytes:bias length:biasLength atIndex:1];
  [commandEncoder setBytes:size length:0 atIndex:2];
  
  if(outToBuf) {
    [commandEncoder setBuffer:_outBuffer offset:0 atIndex:3];
  }
  if(_threadgroupMemoryLength0 > 0) {
    [commandEncoder setThreadgroupMemoryLength:_threadgroupMemoryLength0 atIndex:0];
  }

  //config weight and bias
  [commandEncoder dispatchThreadgroups:_threadGroups threadsPerThreadgroup:_threadsPerThreadGroup];
  [commandEncoder endEncoding];
  [_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
#if TARGET_OS_OSX
      //xcode 10.1 not support
#else
        CFTimeInterval startTime = [commandBuffer GPUStartTime];
        CFTimeInterval endTime = [commandBuffer GPUEndTime];
        double t = endTime - startTime;
        NSLog(@"gpu processing time %f\n",t);
#endif
      }];
  [_commandBuffer commit];
  
  if(waitCompleted) {
    [_commandBuffer waitUntilCompleted];
  }
  return true;
}

- (bool) tuning: (bool)outToBuf width:(int)w height:(int)h
{
  __block double totalTime = 999999.0;
  __block int bestW = 0;
  __block int bestH = 0;
  NSUInteger maxTotalThreadPreThreadGroup = _pipeline.maxTotalThreadsPerThreadgroup;
  for(int j = 1; j < maxTotalThreadPreThreadGroup; j++) {
    for(int k = 1; k < maxTotalThreadPreThreadGroup; k++) {
      if(j*k > maxTotalThreadPreThreadGroup) {
        continue;
      }
      _commandBuffer = [_context.commandQueue commandBuffer];
      id<MTLComputeCommandEncoder> commandEncoder = [_commandBuffer computeCommandEncoder];
      [commandEncoder setComputePipelineState:_pipeline];
      [commandEncoder setTexture:_inTexture atIndex:0];
      if(!outToBuf) {
        [commandEncoder setTexture:_outTexture atIndex:1];
      }
      [commandEncoder setBytes:weights length:weightLength atIndex:0];
      [commandEncoder setBytes:bias length:biasLength atIndex:1];
      [commandEncoder setBytes:size length:0 atIndex:2];
      //[commandEncoder setBuffer:_weights offset:0 atIndex:0];
      //[commandEncoder setBuffer:_bias offset:0 atIndex:1];
      //[commandEncoder setBuffer:_size offset:0 atIndex:2];
      if(outToBuf) {
        [commandEncoder setBuffer:_outBuffer offset:0 atIndex:3];
      }
      if(_threadgroupMemoryLength0 > 0) {
        [commandEncoder setThreadgroupMemoryLength:_threadgroupMemoryLength0 atIndex:0];
      }
      //config weight and bias
      MTLSize threadsPerThreadGroup = MTLSizeMake(j, k, 1);
      MTLSize threadGroups = MTLSizeMake(((w+2)/3 + threadsPerThreadGroup.width - 1) / threadsPerThreadGroup.width,
                                         (h + threadsPerThreadGroup.height -1) / threadsPerThreadGroup.height,
                                         1);
      [commandEncoder dispatchThreadgroups:threadGroups threadsPerThreadgroup:threadsPerThreadGroup];
      [commandEncoder endEncoding];
      [_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
#if TARGET_OS_OSX
        //xcode 10.1 not support
#else
        CFTimeInterval startTime = [commandBuffer GPUStartTime];
        CFTimeInterval endTime = [commandBuffer GPUEndTime];
        double t = endTime - startTime;
        if(t < totalTime) {
          totalTime = t;
          bestW = j;
          bestH = k;
        }
#endif
        //NSLog(@"gpu processing time %f\n",t);
      }];
      [_commandBuffer commit];
      [_commandBuffer waitUntilCompleted];
    }
  }
  NSLog(@"best threadgroup size w = %d h = %d kernel execution time %f \n", bestW, bestH, totalTime);

  
  return true;
}

@end
 
