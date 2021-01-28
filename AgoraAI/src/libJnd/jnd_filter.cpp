#include <cstring>
#include "jnd_filter.h"
#include "jnd_funcs.h"
#include "core/utils.h"

#if __APPLE__
#else
#include <omp.h>
#endif

#ifdef PLATFORM_X86
static InstructionSet _instruction_set;
#endif

typedef void(*BLK_PAD_TO_FLOAT_FUNC)(unsigned char *in, float *blk, int img_w, int img_h, int stride, int blk_x, int blk_y, int num_blk_x, int num_blk_y, int pad);
typedef void(*BLK_CONV1_HWCN_FUNC)(float *in_data, float *out_data, float *weight, float *bias);
typedef void(*BLK_CONV2_HWCN_FUNC)(float *in_data, float *out_data, float *weight, float *bias);
typedef void(*BLK_CONV3_HWCN_FUNC)(float *in_data, unsigned char *out_data, float *weight, float *bias, int img_w, int img_h, int stride, int blk_x, int blk_y);


JndFilter::JndFilter()
{
	_b_init = false;

	_p_temp_buf = 0;
	_n_threads = 2;

	_error_str = "";
}

JndFilter::~JndFilter()
{
	release();
}

bool JndFilter::init(JND_NET_PARAS net_paras, int blk_sz_x, int blk_sz_y, int n_threads)
{
	blk_sz_x = MAX2(blk_sz_x, 16);
	blk_sz_x = MIN2(blk_sz_x, 64);

	blk_sz_y = MAX2(blk_sz_y, 16);
	blk_sz_y = MIN2(blk_sz_y, 64);

	n_threads = MAX2(n_threads, 1);
	n_threads = MIN2(n_threads, 8);

	printf("blk_sz_x:%d, blk_sz_y:%d, nthreads:%d\n", blk_sz_x, blk_sz_y, n_threads);

	_n_threads = n_threads;
	_blk_sz_x = blk_sz_x;
	_blk_sz_y = blk_sz_y;

	update_net_paras(net_paras);

	_b_init = false;

#ifdef PLATFORM_X86
	_b_init =  _instruction_set.FMA();
	_b_init &=  _instruction_set.AVX();
	if (!_b_init)
	{
		_error_str = "No FMA or AVX Instruction Supported!";
		cout << _error_str << endl;		

		return false;
	}
#endif

	BLK_PAD_TO_FLOAT_FUNC f0;	
	BLK_CONV1_HWCN_FUNC   f1;
	BLK_CONV2_HWCN_FUNC   f2;
	BLK_CONV3_HWCN_FUNC   f3;

	if (blk_sz_x >= 64 && blk_sz_y >= 32)
	{
#ifdef PLATFORM_ARM
		f0 = blk_pad_to_float_neon<64, 32>;
		f1 = blk_conv1_hwcn_neon<64, 32>;
		f2 = blk_conv2_hwcn_neon<64, 32>;
		f3 = blk_conv3_hwcn_neon<64, 32>;
#elif defined PLATFORM_X86
		f0 = blk_pad_to_float_avx<64, 32>;
		f1 = blk_conv1_hwcn_avx<64, 32>;
		f2 = blk_conv2_hwcn_avx<64, 32>;
		f3 = blk_conv3_hwcn_avx<64, 32>;
#else
		f0 = blk_pad_to_float<64, 32>;
		f1 = blk_conv1_hwcn<64, 32>;
		f2 = blk_conv2_hwcn<64, 32>;
		f3 = blk_conv3_hwcn<64, 32>;
#endif
	}
	else if (blk_sz_x >= 32 && blk_sz_y >= 16)
	{
#ifdef PLATFORM_ARM
		f0 = blk_pad_to_float_neon<32, 16>;
		f1 = blk_conv1_hwcn_neon<32, 16>;
		f2 = blk_conv2_hwcn_neon<32, 16>;
		f3 = blk_conv3_hwcn_neon<32, 16>;
#elif defined PLATFORM_X86
		f0 = blk_pad_to_float_avx<32, 16>;
		f1 = blk_conv1_hwcn_avx<32, 16>;
		f2 = blk_conv2_hwcn_avx<32, 16>;
		f3 = blk_conv3_hwcn_avx<32, 16>;
#else
		f0 = blk_pad_to_float<32, 16>;
		f1 = blk_conv1_hwcn<32, 16>;
		f2 = blk_conv2_hwcn<32, 16>;
		f3 = blk_conv3_hwcn<32, 16>;
#endif
	}
	else
	{
#ifdef PLATFORM_ARM
		f0 = blk_pad_to_float_neon<16, 16>;
		f1 = blk_conv1_hwcn_neon<16, 16>;
		f2 = blk_conv2_hwcn_neon<16, 16>;
		f3 = blk_conv3_hwcn_neon<16, 16>;
#elif defined PLATFORM_X86
		f0 = blk_pad_to_float_avx<16, 16>;
		f1 = blk_conv1_hwcn_avx<16, 16>;
		f2 = blk_conv2_hwcn_avx<16, 16>;
		f3 = blk_conv3_hwcn_avx<16, 16>;
#else
		f0 = blk_pad_to_float<16, 16>;
		f1 = blk_conv1_hwcn<16, 16>;
		f2 = blk_conv2_hwcn<16, 16>;
		f3 = blk_conv3_hwcn<16, 16>;
#endif
	}

	_funcs[0] = (void*)(f0);
	_funcs[1] = (void*)(f1);
	_funcs[2] = (void*)(f2);
	_funcs[3] = (void*)(f3);

	_b_init = true;

	return _b_init;
}

bool JndFilter::update_net_paras(JND_NET_PARAS net_paras)
{
	int extra_mem_float_size_0 = (_blk_sz_y + 6)*(_blk_sz_x + 6) * 4;
	int extra_mem_float_size_1 = (_blk_sz_y + 4)*(_blk_sz_x + 4) * 4;
	int extra_mem_float_total_size = extra_mem_float_size_0 + extra_mem_float_size_1;
	int extra_net_para_float_size = 36 + 4 + 144 + 4 + 36 + 1;

	if (_p_temp_buf == 0)
	{
		_p_temp_buf = new float[extra_mem_float_total_size*_n_threads + extra_net_para_float_size + 4];
	}

	float *pstart = (float *)(((size_t)_p_temp_buf + 15) / 16 * 16);

	float *conv1_weight = pstart + extra_mem_float_total_size*_n_threads;
	float *conv1_bias = conv1_weight + 36;
	float *conv2_weight = conv1_bias + 4;
	float *conv2_bias = conv2_weight + 144;
	float *conv3_weight = conv2_bias + 4;
	float *conv3_bias = conv3_weight + 36;

	nchw_2_hwcn(net_paras.conv1_weight, conv1_weight, 4, 3, 3, 1);
	memcpy(conv1_bias, net_paras.conv1_bias, 4 * sizeof(float));
	nchw_2_hwcn(net_paras.conv2_weight, conv2_weight, 4, 3, 3, 4);
	memcpy(conv2_bias, net_paras.conv2_bias, 4 * sizeof(float));
	nchw_2_hwcn(net_paras.conv3_weight, conv3_weight, 1, 3, 3, 4);
	memcpy(conv3_bias, net_paras.conv3_bias, 1 * sizeof(float));

	return true;
}

bool JndFilter::release()
{
	if (_p_temp_buf)
	{
		delete[]_p_temp_buf;
		_p_temp_buf = 0;
	}

	return true;
}

bool JndFilter::process(unsigned char *in, unsigned char *out, int w, int h, int stride)
{
#if __APPLE__
#else
	if (!_b_init)
	{
		_error_str = "Not Initialized!";
		cout << _error_str << endl;

		return false;
	}

	if (_p_temp_buf == 0) return false;

	
	const int BLK_LINE_SIZE = (_blk_sz_x + 6)*4;

	int extra_mem_float_size_0 = (_blk_sz_y + 6)*(_blk_sz_x + 6) * 4;
	int extra_mem_float_size_1 = (_blk_sz_y + 4)*(_blk_sz_x + 4) * 4;
	int extra_mem_float_total_size = extra_mem_float_size_0 + extra_mem_float_size_1;
	int extra_net_para_float_size = 36 + 4 + 144 + 4 + 36 + 1;

	float *pstart = (float *)(((size_t)_p_temp_buf + 15) / 16 * 16);

	float *conv1_weight = pstart + extra_mem_float_total_size*_n_threads;
	float *conv1_bias   = conv1_weight + 36;
	float *conv2_weight = conv1_bias + 4;
	float *conv2_bias   = conv2_weight + 144;
	float *conv3_weight = conv2_bias + 4;
	float *conv3_bias   = conv3_weight + 36;

	int num_blk_y = (h + _blk_sz_y - 1) / _blk_sz_y;
	int num_blk_x = (w + _blk_sz_x - 1) / _blk_sz_x;

#pragma omp parallel for num_threads(_n_threads) schedule(dynamic)
	for (int i = 0; i < num_blk_y*num_blk_x; i++)
	{
		int nThreadId =  omp_get_thread_num();

		float *pblkbuf0 = pstart + nThreadId*extra_mem_float_total_size;
		float *pblkbuf1 = pblkbuf0 + extra_mem_float_size_0;

		int blk_y = i / num_blk_x;
		int blk_x = i % num_blk_x;

		(BLK_PAD_TO_FLOAT_FUNC(_funcs[0]))(in, pblkbuf0, w, h, stride, blk_x, blk_y, num_blk_x, num_blk_y, 3);
		(BLK_CONV1_HWCN_FUNC(_funcs[1]))(pblkbuf0, pblkbuf1, conv1_weight, conv1_bias);
		(BLK_CONV2_HWCN_FUNC(_funcs[2]))(pblkbuf1, pblkbuf0, conv2_weight, conv2_bias);
		(BLK_CONV3_HWCN_FUNC(_funcs[3]))(pblkbuf0, out, conv3_weight, conv3_bias, w, h, stride, blk_x, blk_y);
	}
#endif
	return true;
}



