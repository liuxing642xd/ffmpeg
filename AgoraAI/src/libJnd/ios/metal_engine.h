#import <Metal/Metal.h>

@interface MetalEngine : NSObject

+ (instancetype)defaultContext;
- (instancetype)initWithDevice:(id<MTLDevice>)device;

@property (nonatomic, strong) id<MTLDevice> device;
@property (nonatomic, strong) id<MTLLibrary> library;
@property (nonatomic, strong) id<MTLCommandQueue> commandQueue;

@end
