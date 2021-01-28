#include "Jnd4_Padding_ConvsAndBias_paras.h"
#include "jnd_filter_wrapper.h"
#include "jnd_filter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HELPER_JND_EXPORT __attribute__ ((visibility ("default")))

static void FillParas(JND_NET_PARAS *paras, int qp)
{
    if (qp <= 24) {
        paras->conv1_weight = jnd4_22_padding_conv1_weight;
        paras->conv1_bias = jnd4_22_padding_conv1_bias;
        paras->conv2_weight = jnd4_22_padding_conv2_weight;
        paras->conv2_bias = jnd4_22_padding_conv2_bias;
        paras->conv3_weight = jnd4_22_padding_conv3_weight;
        paras->conv3_bias = jnd4_22_padding_conv3_bias;
    } else if (qp <= 30) {
        paras->conv1_weight = jnd4_27_padding_conv1_weight;
        paras->conv1_bias = jnd4_27_padding_conv1_bias;
        paras->conv2_weight = jnd4_27_padding_conv2_weight;
        paras->conv2_bias = jnd4_27_padding_conv2_bias;
        paras->conv3_weight = jnd4_27_padding_conv3_weight;
        paras->conv3_bias = jnd4_27_padding_conv3_bias;
    } else if (qp <= 34) {
        paras->conv1_weight = jnd4_32_padding_conv1_weight;
        paras->conv1_bias = jnd4_32_padding_conv1_bias;
        paras->conv2_weight = jnd4_32_padding_conv2_weight;
        paras->conv2_bias = jnd4_32_padding_conv2_bias;
        paras->conv3_weight = jnd4_32_padding_conv3_weight;
        paras->conv3_bias = jnd4_32_padding_conv3_bias;
    } else /*if (qp <= 40)*/ {
        paras->conv1_weight = jnd4_37_padding_conv1_weight;
        paras->conv1_bias = jnd4_37_padding_conv1_bias;
        paras->conv2_weight = jnd4_37_padding_conv2_weight;
        paras->conv2_bias = jnd4_37_padding_conv2_bias;
        paras->conv3_weight = jnd4_37_padding_conv3_weight;
        paras->conv3_bias = jnd4_37_padding_conv3_bias;
    }
}

void* HELPER_JND_EXPORT CreateAndInitJndFilter(int qp, int threads)
{
    JndFilter *filter = new JndFilter;

    JND_NET_PARAS jnd_net_paras;
    FillParas(&jnd_net_paras, qp);
    if (!filter->init(jnd_net_paras, 64, 32, threads)) {
        delete filter;
        return nullptr;
    }

    return (void *)filter;
}

int HELPER_JND_EXPORT JndFilterProcess(void *filter, unsigned char *in, unsigned char *out, int width, int height, int stride)
{
    JndFilter *filter_ = (JndFilter *)filter;

    if (filter_->process(in, out, width, height, stride)) 
        return 0;
    else
        return -1;
}

int HELPER_JND_EXPORT JndFilterUpdate(void *filter, int qp)
{
    JndFilter *filter_ = (JndFilter *)filter;

    JND_NET_PARAS jnd_net_paras;
    FillParas(&jnd_net_paras, qp);
    if (filter_->update_net_paras(jnd_net_paras))
        return 0;
    else
        return -1;
}

void HELPER_JND_EXPORT DestoryJndFilter(void *filter)
{
    JndFilter *filter_ = (JndFilter *)filter;

    filter_->release();
    delete filter_;
}

#ifdef __cplusplus
}
#endif