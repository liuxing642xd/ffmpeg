

#include "core\net.h"
#include "core\utils.h"
#include "impl_ref\pad.h"
#include "impl_ref\conv.h"
#include "impl_ref\relu.h"
#include "cl_engine.h"
#include "KernelMacro.h"

const int width = 1280;
const int height = 720;

void transform_weight(float *in, int n, int h, int w, int c)
{
	float *temp = new float[n*h*w*c];

	//nchw_2_nhwc(in, temp, n, h, w, c);

	nchw_2_hwcn(in, temp, n, h, w, c);

	int sz = n*h*w*c;

	for (int i = 0; i < sz; i += 4)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%8ff,", temp[i + j]);
		}
		printf("\n");
	}

	delete[]temp;
}


#include "jnd_model.h"
#include "jnd_filter.h"

ENGINE_DEVICE_TYPE g_device_type = ENGINE_DEVICE_GPU_NVIDIA;

bool test_jnd(unsigned char *in, unsigned char *out)
{
	Net net;
	int sz = width*height;

	float *fIn = new float[sz];
	for (int i = 0; i < sz; i++)
	{
		fIn[i] = in[i] / 255.0f;
	}

	std::vector<Tensor> &dt = net.get_dynamic_tensors();
	dt.resize(5);

	std::vector<Tensor> &st = net.get_static_tensors();
	st.resize(6);

	dt[0].reshape(1, height, width, 1, NHWC, fIn);

	st.resize(6);
	st[0].reshape(4, 3, 3, 1, NHWC, conv1_weight_nhwc);
	st[1].reshape(1, 1, 1, 4, NHWC, conv1_bias);
	st[2].reshape(4, 3, 3, 4, NHWC, conv2_weight_nhwc);
	st[3].reshape(1, 1, 1, 4, NHWC, conv2_bias);
	st[4].reshape(1, 3, 3, 4, NHWC, conv3_weight_nhwc);
	st[5].reshape(1, 1, 1, 1, NHWC, conv3_bias);

	ref::Pad   *pad_1   = new ref::Pad("pad0", &dt[0], &dt[1], 3, 3, 3, 3);
	ConvParam  conv_para = { 3, 3, 0, 0, 0, 0, 1, 1 };
	ref::Conv  *conv_1 = new ref::Conv("conv1", &dt[1], &dt[2], &st[0], &st[1], conv_para);
	ref::Relu  *relu_1 = new ref::Relu("relu1", &dt[2], &dt[2]);

	ref::Conv  *conv_2 = new ref::Conv("conv2", &dt[2], &dt[3], &st[2], &st[3], conv_para);
	ref::Relu  *relu_2 = new ref::Relu("relu2", &dt[3], &dt[3]);

	ref::Conv  *conv_3 = new ref::Conv("conv3", &dt[3], &dt[4], &st[4], &st[5], conv_para);

	net.add_layer(pad_1);
	net.add_layer(conv_1);
	net.add_layer(relu_1);
	net.add_layer(conv_2);
	net.add_layer(relu_2);
	net.add_layer(conv_3);

	net.warmup();

	net.run();

	float *fOut = dt[4].f32();

	for (int i = 0; i < sz; i++)
	{
		out[i] = (unsigned char)(fOut[i]*255.0f);
	}

	delete[]fIn;


	//float *temp0 = new float[(1280 + 6)*(720 + 6)*4];
	//float *temp1 = new float[(1280 + 4)*(720 + 4)*4];
	//float *temp2 = new float[(1280 + 2)*(720 + 2)*4];
	//float *temp3 = new float[(1280)*(720)];

	//blk_pad_to_float(in, temp0, 1280, 720, 0, 0, 3);
	//blk_conv1(temp0, temp1, conv1_weight_nhwc, conv1_bias);
	//blk_conv2(temp1, temp2, conv2_weight_nhwc, conv2_bias);

	//float dif0 = compare(temp0, dt[1].f32(), dt[1].size());
	//float dif1 = compare(temp1, dt[2].f32(), dt[2].size());
	//float dif2 = compare(temp2, dt[3].f32(), dt[3].size());

	//printf("diff:%f, %f, %f\n", dif0, dif1, dif2);

	//delete[]temp0;
	//delete[]temp1;
	//delete[]temp2;
	//delete[]temp3;

	return 0;
}

bool test_jnd_cl_conv1()
{
	Net net;
	int sz = width*height;

	float *fIn = new float[sz];
	float *fOut = new float[sz*4];
	for (int i = 0; i < sz; i++)
	{
		fIn[i] = (i & 0xff) / 255.0f;
	}

	std::vector<Tensor> &dt = net.get_dynamic_tensors();
	dt.resize(2);

	std::vector<Tensor> &st = net.get_static_tensors();
	st.resize(2);

	//for (int i = 0; i < 36; i++)
	//{
	//	conv1_weight_nhwc[i] = 1.0f;
	//	conv1_weight[i] = 1.0f;
	//}
	//for (int i = 0; i < 4; i++)
	//{
	//	conv1_bias[i] = 1.0f;
	//}

	dt[0].reshape(1, height, width, 1, NHWC, fIn);
	st[0].reshape(4, 3, 3, 1, NHWC, conv1_weight_nhwc);
	st[1].reshape(1, 1, 1, 4, NHWC, conv1_bias);

	ConvParam  conv_para = { 3, 3, 1, 1, 1, 1, 1, 1 };
	ref::Conv  *conv_1 = new ref::Conv("conv1", &dt[0], &dt[1], &st[0], &st[1], conv_para);
	ref::Relu  *relu_1 = new ref::Relu("relu1", &dt[1], &dt[1]);
	net.add_layer(conv_1);
	net.add_layer(relu_1);

	net.warmup();
	net.run();

	cl_int status = CL_SUCCESS;
	clEngine cl_engine;
	cl_engine.Init(g_device_type);
	cl_context context = cl_engine.GetContext();
	cl_command_queue queue = cl_engine.GetQueue();
	vector<string> files;
	files.push_back("../Test/Test.cl");
	bool res = cl_engine.CompileSource(files);
	if (res == false)
	{
		string errStr = cl_engine.GetErrorStr();
		std::cout << errStr << endl;
	}

	cl_kernel kernel = cl_engine.FindKernel("jnd_conv1_3x3_s1x1_n4_c1");
	if (kernel == 0)
	{
		printf("can not find jnd_conv1_3x3_s1x1_n4_c1 kernel\n");
		return false;
	}

	cl_mem in_img = cl_engine.CreateImage2D<CL_FLOAT>(width/4, height, CL_MEM_READ_WRITE);
	cl_mem out_img = cl_engine.CreateImage2D<CL_FLOAT>(width, height, CL_MEM_READ_WRITE);
	cl_mem weight_img = cl_engine.CreateImage2D<CL_FLOAT>(1, 37);

	float temp_weight_bias[36 + 4];
	nchw_2_hwcn(conv1_weight, temp_weight_bias, 4, 3, 3, 1);
	memcpy(temp_weight_bias + 36, conv1_bias, 4 * sizeof(float));


	{
		TimeProfiler t("WriteImage2D");
		cl_engine.WriteImage2D(in_img, fIn, width/4, height);
		cl_engine.WriteImage2D(weight_img, temp_weight_bias, 1, 10);
	}

	SetKernelArg5(kernel, in_img, out_img, weight_img, width, height);

	CHECK_CL_ERROR(status);

	size_t localWorkSize[2] = { 16, 16 };
	size_t globalWorkSize[2] = { ROUND_UP(localWorkSize[0], width / 4), ROUND_UP(localWorkSize[1], height) };
	//size_t globalWorkSize[2] = { width / 4, height };
	{
		TimeProfiler t("Kernel Execution");
		status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
		clFinish(queue);
	}

	CHECK_CL_ERROR(status);

	{
		TimeProfiler t("ReadImage2D");
		cl_engine.ReadImage2D(out_img, fOut, width, height);
	}

	float *ref_conv_out = dt[1].f32();
	float dif2 = compare(fOut, ref_conv_out, dt[1].size());
	dif2 /= (width*height*4);
	//for (int i = 0; i < 16; i++)
	//{
	//	printf("%f,", fOut[13764+i]);
	//}
	
	printf("diff2:%f\n", dif2);


	clReleaseMemObject(in_img);
	clReleaseMemObject(out_img);
	clReleaseMemObject(weight_img);
	delete[]fIn;
	delete[]fOut;

	return true;
}


bool test_jnd_cl_conv2()
{
	Net net;
	int sz = width*height*4;

	float *fIn  = new float[sz];
	float *fOut = new float[sz];
	for (int i = 0; i < sz; i++)
	{
		fIn[i] = (i&0xff) / 255.0f;
	}

	std::vector<Tensor> &dt = net.get_dynamic_tensors();
	dt.resize(2);

	std::vector<Tensor> &st = net.get_static_tensors();
	st.resize(2);


	dt[0].reshape(1, height, width, 4, NHWC, fIn);	
	st[0].reshape(4, 3, 3, 4, NHWC, conv2_weight_nhwc);
	st[1].reshape(1, 1, 1, 4, NHWC, conv2_bias);
	
	ConvParam  conv_para = { 3, 3, 1, 1, 1, 1, 1, 1 };
	ref::Conv  *conv_2 = new ref::Conv("conv2", &dt[0], &dt[1], &st[0], &st[1], conv_para);
	ref::Relu  *relu_2 = new ref::Relu("relu2", &dt[1], &dt[1]);	
	net.add_layer(conv_2);
	net.add_layer(relu_2);

	net.warmup();
	net.run();

	cl_int status = CL_SUCCESS;
	clEngine cl_engine;
	cl_engine.Init(g_device_type);
	cl_context context = cl_engine.GetContext();
	cl_command_queue queue = cl_engine.GetQueue();
	vector<string> files;
	files.push_back("../Test/Test.cl");
	bool res = cl_engine.CompileSource(files);
	if (res == false)
	{
		string errStr = cl_engine.GetErrorStr();
		std::cout << errStr << endl;
	}

	cl_kernel kernel = cl_engine.FindKernel("jnd_conv2_3x3_s1x1_n4_c4");
	if (kernel == 0)
	{
		printf("can not find jnd_conv2_3x3_s1x1_n4_c4 kernel\n");
		return false;
	}

	cl_mem in_img     = cl_engine.CreateImage2D<CL_FLOAT>(width, height, CL_MEM_READ_WRITE);
	cl_mem out_img    = cl_engine.CreateImage2D<CL_FLOAT>(width, height, CL_MEM_READ_WRITE);
	cl_mem weight_img = cl_engine.CreateImage2D<CL_FLOAT>(1, 37, CL_MEM_READ_WRITE);

	float temp_weight_bias[36 * 4 + 4];
	nchw_2_hwcn(conv2_weight, temp_weight_bias, 4, 3, 3, 4);	
	memcpy(temp_weight_bias + 36 * 4, conv2_bias, 4 * sizeof(float));


	{
		TimeProfiler t("WriteImage2D");
		cl_engine.WriteImage2D(in_img, fIn, width, height);
		cl_engine.WriteImage2D(weight_img, temp_weight_bias, 1, 37);
	}

	SetKernelArg5(kernel, in_img, out_img, weight_img, width, height);

	CHECK_CL_ERROR(status);

	size_t localWorkSize[2] = { 16, 16 };
	size_t globalWorkSize[2] = { ROUND_UP(localWorkSize[0], width / 4), ROUND_UP(localWorkSize[1], height) };

	{
		TimeProfiler t("Kernel Execution");
		status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
		clFinish(queue);
	}

	CHECK_CL_ERROR(status);

	{
		TimeProfiler t("ReadImage2D");
		cl_engine.ReadImage2D(out_img, fOut, width, height);
	}

	float *ref_conv_out = dt[1].f32();
	float dif2 = compare(fOut, ref_conv_out, dt[1].size());
	//for (int i = 0; i < 16; i++)
	//{
	//	printf("%f,", fOut[13764 + i]);
	//}

	dif2 /= (width*height*4);
	printf("diff2:%f\n", dif2);


	clReleaseMemObject(in_img);
	clReleaseMemObject(out_img);
	clReleaseMemObject(weight_img);
	delete[]fIn;
	delete[]fOut;
	
	return true;
}

bool test_jnd_cl_conv3()
{
	Net net;
	int sz = width*height;

	float    *fIn  = new float[sz*4];
	float    *fOut = new float[sz];
		
	for (int i = 0; i < sz*4; i++)
	{
		fIn[i] = (i&0xff) / 255.0f;
	}

	std::vector<Tensor> &dt = net.get_dynamic_tensors();
	dt.resize(2);

	std::vector<Tensor> &st = net.get_static_tensors();
	st.resize(2);


	dt[0].reshape(1, height, width, 4, NHWC, fIn);
	st[0].reshape(1, 3, 3, 4, NHWC, conv3_weight_nhwc);
	st[1].reshape(1, 1, 1, 1, NHWC, conv3_bias);

	ConvParam  conv_para = { 3, 3, 1, 1, 1, 1, 1, 1 };
	ref::Conv  *conv_3 = new ref::Conv("conv3", &dt[0], &dt[1], &st[0], &st[1], conv_para);
	
	net.add_layer(conv_3);
	
	net.warmup();
	net.run();

	cl_int status = CL_SUCCESS;
	clEngine cl_engine;
	cl_engine.Init(g_device_type);
	cl_context context = cl_engine.GetContext();
	cl_command_queue queue = cl_engine.GetQueue();
	vector<string> files;
	files.push_back("../Test/Test.cl");
	bool res = cl_engine.CompileSource(files);
	if (res == false)
	{
		string errStr = cl_engine.GetErrorStr();
		std::cout << errStr << endl;
	}

	cl_kernel kernel = cl_engine.FindKernel("jnd_conv3_3x3_s1x1_n1_c4");
	if (kernel == 0)
	{
		printf("can not find jnd_conv3_3x3_s1x1_n1_c4 kernel\n");
		return false;
	}

	cl_mem in_img = cl_engine.CreateImage2D<CL_FLOAT>(width, height, CL_MEM_READ_WRITE);
	cl_mem out_img = cl_engine.CreateImage2D<CL_FLOAT>(width/4, height, CL_MEM_READ_WRITE);
	cl_mem weight_img = cl_engine.CreateImage2D<CL_FLOAT>(1, 10, CL_MEM_READ_WRITE);

	float temp_weight_bias[36 + 4];
	nchw_2_hwcn(conv3_weight, temp_weight_bias, 1, 3, 3, 4);
	for (int i = 0; i < 4; i++)
	{
		temp_weight_bias[36 + i] = conv3_bias[0];
	}


	{
		TimeProfiler t("WriteImage2D");
		cl_engine.WriteImage2D(in_img, fIn, width, height);
		cl_engine.WriteImage2D(weight_img, temp_weight_bias, 1, 10);
	}

	SetKernelArg5(kernel, in_img, out_img, weight_img, width, height);

	CHECK_CL_ERROR(status);

	size_t localWorkSize[2] = { 16, 16 };
	size_t globalWorkSize[2] = { ROUND_UP(localWorkSize[0], width / 4), ROUND_UP(localWorkSize[1], height) };

	{
		TimeProfiler t("Kernel Execution");
		status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
		clFinish(queue);
	}

	CHECK_CL_ERROR(status);

	{
		TimeProfiler t("ReadImage2D");
		cl_engine.ReadImage2D(out_img, fOut, width/4, height);
	}

	//float *ref_conv_out = dt[1].f32();
	//for (int i = 0; i < dt[1].size(); i++)
	//{
	//	ucOutRef[i] = (unsigned char)(ref_conv_out[i] * 255.0f);
	//}

	float *ref_conv_out = dt[1].f32();
	float dif2 = compare(fOut, ref_conv_out, dt[1].size());
	//for (int i = 0; i < 16; i++)
	//{
	//	printf("%f,", fOut[13764 + i]);
	//}

	dif2 /= (width*height);
	printf("diff2:%f\n", dif2);


	clReleaseMemObject(in_img);
	clReleaseMemObject(out_img);
	clReleaseMemObject(weight_img);
	delete[]fIn;
	delete[]fOut;
//	delete[]ucOutCL;
//	delete[]ucOutRef;

	return true;
}