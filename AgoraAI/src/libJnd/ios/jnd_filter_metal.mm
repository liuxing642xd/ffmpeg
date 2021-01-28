#import <Foundation/Foundation.h>
#import "jnd_filter_oc.h"

#include "jnd_filter_metal.h"


struct JNDFilterImpl {
   JNDFilter* filter;
};

JndFilterMetal::JndFilterMetal():
  impl(new JNDFilterImpl())
{
  impl->filter = [[JNDFilter alloc] init];
}

JndFilterMetal::~JndFilterMetal()
{
  if (impl) {
    [impl->filter unInit];
    delete impl;
    impl = nullptr;
  }
}

bool JndFilterMetal::init(JND_NET_PARAS net_paras)
{
  update_net_paras(net_paras);
  return true;
}

bool JndFilterMetal::release()
{
  [impl->filter unInit];
  return true;
}

bool JndFilterMetal::process(unsigned char *input, unsigned char *output, int w, int h, int stride)
{
  bool result = false;
  {
    double start = clock();
    TimeProfiler t("process");
    result = [impl->filter process:input output:output width:w height:h stride:stride];
    double cost_t = clock() - start;
    NSLog(@"cost process cost time = %f", cost_t / CLOCKS_PER_SEC);
  }
  return result;
}

bool JndFilterMetal::tuning(unsigned char *input, unsigned char *output, int w, int h, int stride)
{
  return [impl->filter tuning:input output:output width:w height:h stride:stride];
;
}


bool JndFilterMetal::update_net_paras(JND_NET_PARAS net_paras)
{
  return [impl->filter update_net_paras:net_paras];
}

bool JndFilterMetal::prepare(int w, int h, int stride, unsigned char *input, unsigned char *output)
{
  return [impl->filter prepare:w height:h stride:stride input:input output:output];
}
