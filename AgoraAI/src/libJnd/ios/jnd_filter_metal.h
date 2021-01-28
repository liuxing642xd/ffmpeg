#pragma once

typedef struct _JND_NET_PARAS_
{
  float *conv1_weight;
  float *conv1_bias;
  float *conv2_weight;
  float *conv2_bias;
  float *conv3_weight;
  float *conv3_bias;
}JND_NET_PARAS;

struct JNDFilterImpl;

class JndFilterMetal
{
public:
  JndFilterMetal();
  virtual ~JndFilterMetal();

  bool init(JND_NET_PARAS net_paras);

  virtual bool release();

  virtual bool process(unsigned char *input, unsigned char *output, int w, int h, int stride);

  virtual bool update_net_paras(JND_NET_PARAS net_paras); // not thread safe
  
  virtual bool tuning(unsigned char *input, unsigned char *output, int w, int h, int stride);
  
private:
  bool prepare(int w, int h, int stride, unsigned char *input, unsigned char *output);

private:
  JNDFilterImpl* impl;
};
