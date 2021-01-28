#import "metal_engine.h"

@implementation MetalEngine

+ (instancetype)defaultContext {
    static dispatch_once_t onceToken;
    static MetalEngine *instance = nil;
    dispatch_once(&onceToken, ^{
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        instance = [[MetalEngine alloc] initWithDevice:device];
    });

    return instance;
}

- (instancetype)initWithDevice:(id<MTLDevice>)device {
    if ((self = [super init])) {
        _device = device;
        //_library = [_device newDefaultLibrary];
        NSBundle* bundle = NSBundle.mainBundle;
        NSURL* assetPath = [bundle URLForResource:@"pvc_kernels" withExtension:@"metallib"];
        NSError* error = nil;
        _library = [_device newLibraryWithURL:assetPath error:&error];
        if(error != nil) {
          NSLog(@"new pvc kernels metal lib error \n");
        }
        _commandQueue = [_device newCommandQueue];
    }

    return self;
}

@end
