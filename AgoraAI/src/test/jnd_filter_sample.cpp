#include "core/utils.h"
#include "jnd_filter.h"
#include "jnd_filter_cl.h"

float jnd4_27_padding_1280x720_conv1_weight[36] =
{ 
4.6795e-03,  2.1189e-02,  2.3258e-03,
-1.5122e-02,  4.5112e-01, -1.0797e-02,
4.9031e-01,  1.1472e+00,  4.8470e-01,
8.7700e-04, -5.2115e-03,  4.0341e-02,
-4.8225e-01, -3.9903e-01, -3.1411e-01,
7.4129e-02, -4.8316e-01, -2.8285e-01,
-2.2135e-01, -4.8258e-01, -1.6435e-01,
-6.3413e-01, -5.4598e-01, -1.9944e-01,
-2.1050e-01, -1.0197e-01,  1.0248e-01,
-1.0330e-01,  1.4829e-01, -9.9375e-02,
2.6131e-01,  6.7250e-01,  2.5823e-01,
-1.1168e-01,  1.3641e-01, -1.0990e-01 
};

float jnd4_27_padding_1280x720_conv1_bias[4] = { -1.0371f, -0.2605f, -0.3185f, -0.0787f };

float jnd4_27_padding_1280x720_conv2_weight[144] =
{ 
-0.2315, -0.3104, -0.2723,
-0.2092, -0.2045, -0.1434,
-0.2573, -0.2692, -0.2133,
-0.0883, -0.1036,  0.0438,
0.0669,  0.0735, -0.0977,
-0.2151,  0.0253,  0.0351,
-0.2356, -0.0858, -0.1476,
-0.2394,  0.0359, -0.2254,
-0.2052, -0.1330, -0.1918,
-0.3375, -0.3507, -0.3490,
-0.3205,  0.1134, -0.1857,
-0.4131,  0.0089, -0.1612,
-0.1112, -0.2425, -0.2031,
-0.2700, -0.2391, -0.2663,
-0.3041, -0.2710, -0.2538,
-0.1813,  0.0271, -0.0363,
-0.1884, -0.0564, -0.1068,
-0.1454,  0.0581, -0.0562,
-0.1726, -0.0449, -0.3058,
-0.0304,  0.0899, -0.1889,
-0.1800, -0.0994, -0.2272,
-0.1666, -0.2140, -0.4441,
-0.0932, -0.1352, -0.4123,
-0.2923, -0.6318, -0.4735,
0.0038,  0.0097,  0.0170,
-0.0491, -0.2282, -0.0490,
-0.0372,  0.0406, -0.0414,
-0.1850, -0.0136, -0.1713,
-0.0069, -0.0043,  0.0194,
0.3202,  0.4829,  0.1692,
-0.0988,  0.0797, -0.1054,
-0.1463,  0.0659,  0.1162,
-0.0160,  0.1443,  0.0858,
-0.0332,  0.0391, -0.0300,
0.1054, -0.3886,  0.1055,
-0.0931, -1.0948, -0.0948,
0.0666,  0.6168,  0.0698,
0.0316, -0.0172,  0.0288,
0.0319, -0.0410,  0.0302,
0.1297,  0.0281,  0.3521,
0.2809,  0.1583,  0.1417,
0.2652,  0.3231,  0.1549,
0.5009,  0.2151,  0.4011,
0.3626,  0.0909,  0.2175,
0.3460,  0.1401,  0.1214,
-0.1385,  0.2291, -0.1386,
0.0304,  0.3805,  0.0280,
-0.1443,  0.1702, -0.1392 
};

float jnd4_27_padding_1280x720_conv2_bias[4]  = { -0.1516f, -0.2117f,  0.5518f, -0.1377f };

float jnd4_27_padding_1280x720_conv3_weight[36] = {
	0.2357,  0.4447, -0.2053,
	0.0758,  0.2640, -0.0362,
	-0.0820, -0.0830, -0.1136,
	0.0035, -0.0470,  0.0967,
	0.2364,  0.2500,  0.4845,
	0.0669,  0.3078,  0.1896,
	-0.0495, -0.4181, -0.0506,
	0.0343, -0.1538,  0.0343,
	-0.0139,  0.0169, -0.0136,
	-0.0073,  0.0027, -0.0082,
	0.0203,  0.3282,  0.0195,
	-0.0472,  0.1361, -0.0472 };


float jnd4_27_padding_1280x720_conv3_bias[1] = { 0.4126f };


bool test_jnd_filter_cpu(unsigned char *in, unsigned char *out, int img_w, int img_h, int blk_size_x=64, int blk_size_y=32, int nthreads=2, int nloop=100)
{	
	JndFilter jnd_filter;

	{
		TimeProfiler t("jnd init");
		JND_NET_PARAS paras;
		paras.conv1_weight = jnd4_27_padding_1280x720_conv1_weight;
		paras.conv1_bias = jnd4_27_padding_1280x720_conv1_bias;
		paras.conv2_weight = jnd4_27_padding_1280x720_conv2_weight;
		paras.conv2_bias = jnd4_27_padding_1280x720_conv2_bias;
		paras.conv3_weight = jnd4_27_padding_1280x720_conv3_weight;
		paras.conv3_bias = jnd4_27_padding_1280x720_conv3_bias;

		jnd_filter.init(paras, blk_size_x, blk_size_y, nthreads);
	}

	{
		//for (int k = 0; k < nloop; k++)
		{
			TimeProfiler t("jnd process");
			for (int i = 0; i < nloop; i++)
			{
				jnd_filter.process(in, out, img_w, img_h, img_w);
			}
		}
	}

	{
		TimeProfiler t("jnd release");
		jnd_filter.release();
	}

	return true;
}

bool test_jnd_filter_cl(unsigned char *in, unsigned char *out, int img_w, int img_h, int run_mode = 0, int prefer_dev_type=0, int nloop = 100)
{
	JndFilterCL jnd_filter;
	
	{
		TimeProfiler t("jnd init");
		JND_NET_PARAS paras;
		paras.conv1_weight = jnd4_27_padding_1280x720_conv1_weight;
		paras.conv1_bias = jnd4_27_padding_1280x720_conv1_bias;
		paras.conv2_weight = jnd4_27_padding_1280x720_conv2_weight;
		paras.conv2_bias = jnd4_27_padding_1280x720_conv2_bias;
		paras.conv3_weight = jnd4_27_padding_1280x720_conv3_weight;
		paras.conv3_bias = jnd4_27_padding_1280x720_conv3_bias;

		//jnd_filter.init(paras, "JndCLkernel.bin", run_mode, prefer_dev_type);
		jnd_filter.init(paras, "", run_mode, prefer_dev_type);
	}

	{
		//for (int k = 0; k < nloop; k++)
		{
			TimeProfiler t("jnd process");
			for (int i = 0; i < nloop; i++)
			{
				jnd_filter.process(in, out, img_w, img_h, img_w);
			}
		}
	}

	{
		TimeProfiler t("jnd release");
		jnd_filter.release();
	}



	return true;
}
