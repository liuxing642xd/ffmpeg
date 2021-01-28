
#include <cstring>
#include <stdio.h>
#include <string>
#include "core/common.h"

#ifdef PLATFORM_X86
//#include "vld.h"
#endif

using namespace std;

extern bool test_jnd(unsigned char *in, unsigned char *out);
extern bool test_jnd_filter_cpu(unsigned char *in, unsigned char *out, int img_w, int img_h, int blk_size_x = 64, int blk_size_y = 32, int nthreads = 2, int nloop = 100);
extern bool test_jnd_filter_cl(unsigned char *in, unsigned char *out, int img_w, int img_h, int run_mode = 0, int prefer_dev_type = 0, int nloop = 100);
extern bool test_jnd_cl_conv1();
extern bool test_jnd_cl_conv2();
extern bool test_jnd_cl_conv3();

void dump_one_yuv_frame(const char *infile, const char *outfile)
{
	FILE *fp_in = fopen(infile, "rb");

	int sz = 1280 * 768 * 3 / 2;
	unsigned char *data = new unsigned char[sz];

	fread(data, 1, sz, fp_in);

	
	FILE *fp_out = fopen(outfile, "wb");

	fwrite(data, 1, sz, fp_out);

	fclose(fp_in);
	fclose(fp_out);

	delete[]data;

}


bool crop_yv12(unsigned char*in, unsigned char*out, int in_w, int in_h, int out_w, int out_h)
{
	if (out_w > in_w || out_h > in_h) return false;

	unsigned char *in_y = in;
	unsigned char *in_u = in_y + in_w*in_h;
	unsigned char *in_v = in_u + in_w*in_h/4;

	unsigned char *out_y = out;
	unsigned char *out_u = out_y + out_w*out_h;
	unsigned char *out_v = out_u + out_w*out_h / 4;

	int in_start_y = (in_h - out_h) / 2;
	int in_start_x = (in_w - out_w) / 2;
	for (int y = 0; y < out_h; y++)
	{
		memcpy(out_y + y*out_w, in_y + (y + in_start_y)*in_w + in_start_x, out_w);
	}

	for (int y = 0; y < out_h/2; y++)
	{
		memcpy(out_u + y*out_w / 2, in_u + (y + in_start_y / 2)*in_w / 2 + in_start_x / 2, out_w / 2);
		memcpy(out_v + y*out_w / 2, in_v + (y + in_start_y / 2)*in_w / 2 + in_start_x / 2, out_w / 2);
	}



	return true;
}

#if 0 //def PLATFORM_X86
#define USE_OPENCV
#endif

#ifdef USE_OPENCV
#include "opencv2/opencv.hpp"
#endif

int main(int argc, char *argv[])
{
	const int width = 1280;
	const int height = 720;

	int sz = width*height*3/2;

	FILE *fp_in = fopen("120_org_0.yuv", "rb");
	unsigned char *data_in = new unsigned char[sz];
	fread(data_in, 1, sz, fp_in);
	fclose(fp_in);

	const int crop_width  = 1280;
	const int crop_height = 720;
	sz = crop_width*crop_height * 3 / 2;
	unsigned char *data_crop = new unsigned char[sz];

	crop_yv12(data_in, data_crop, width, height, crop_width, crop_height);

	unsigned char *data_out = new unsigned char[sz];

	int run_type = 0; // 0:CPU, others:CL
	int nloop = 100;
	if (argc > 1)
	{
		sscanf(argv[1], "%d", &run_type);
	}

	if (argc > 2)
	{
		sscanf(argv[2], "%d", &nloop);
	}

	if (run_type == 0)
	{
		int nthreads = 2;
		int blk_sz_x = 64;
		int blk_sz_y = 32;

		if (argc > 3)
		{
			sscanf(argv[3], "%d", &nthreads);
		}
		if (argc > 4)
		{
			sscanf(argv[4], "%d", &blk_sz_x);
		}
		if (argc > 5)
		{
			sscanf(argv[5], "%d", &blk_sz_y);
		}

		test_jnd_filter_cpu(data_crop, data_out, crop_width, crop_height, blk_sz_x, blk_sz_y, nthreads, nloop);
	}
	else
	{
		int run_mode = 0;
		int prefer_dev_type = 0;
		if (argc > 3)
		{
			sscanf(argv[3], "%d", &run_mode);
		}
		if (argc > 4)
		{
			sscanf(argv[4], "%d", &prefer_dev_type);
		}
		test_jnd_filter_cl(data_crop, data_out, crop_width, crop_height, run_mode, prefer_dev_type, nloop);
	}

	memcpy(data_out + crop_width*crop_height, data_crop + crop_width*crop_height, crop_width*crop_height / 2);

#ifdef USE_OPENCV
	cv::Mat yuv(crop_height *3/2, crop_width, CV_8UC1, data_out);
	cv::Mat rgb(crop_height, crop_width, CV_8UC3);
	cv::cvtColor(yuv, rgb, cv::COLOR_YUV2RGB_YV12);
	char out_file_name[256];
	sprintf(out_file_name, "120_out_0_%dx%d.jpg", crop_width, crop_height);
	cv::imwrite(out_file_name, rgb);
#else
	char out_file_name[256];
	sprintf(out_file_name, "120_out_0_%dx%d.yuv", crop_width, crop_height);
	FILE *fp_out = fopen(out_file_name, "wb");
	fwrite(data_out, 1, sz, fp_out);
	fclose(fp_out);
#endif
	//dump_one_yuv_frame("120_vidyo1_1280x720_60_600.yuv", "120_org_0.yuv");
	//dump_one_yuv_frame("120_vidyo1_1280x720_60_600_jnd.yuv", "120_jnd_0.yuv");

	delete[]data_in;
	delete[]data_crop;
	delete[]data_out;

	return 0;
}


//int main(int argc, char *argv[])
//{
//	for(int i=0; i<1; i++)
//	test_jnd_cl_conv1();
//
//	return 0;
//}
//
