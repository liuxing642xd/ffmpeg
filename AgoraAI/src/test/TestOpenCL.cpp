
#include <stdio.h>
#include <string>
#include "KernelMacro.h"
#include "core/common.h"
#include "core/utils.h"
#include "cL_engine.h"
#include <iostream>

int extend_channel_main(int argc, char *argv[])
{
	cl_int status = CL_SUCCESS;
	clEngine cl_engine;
	cl_engine.Init(ENGINE_DEVICE_GPU_INTEL);
//	cl_engine.PrintDeviceInfos();
	cl_context context     = cl_engine.GetContext();
	cl_command_queue queue = cl_engine.GetQueue();
	vector<string> files;
	files.push_back("../Test/Test.cl");
	bool res = cl_engine.CompileSource(files);
	if (res == false)
	{
		string errStr = cl_engine.GetErrorStr();
		std::cout << errStr << endl;
	}


	cl_kernel kernel = cl_engine.FindKernel("extend_channel");
	if (kernel == 0)
	{
		printf("can not find extend_channel kernel\n");
		return -1;
	}

	printf("0\n");

	const int width = 1280;
	const int height = 720;

	int sz = width*height;
	unsigned char *data_in = new unsigned char[sz];
	float *data_out = new float[sz * 4];
	for (int i = 0; i < sz; i++)
	{
		data_in[i] = (i & 0xff);
	}

	cl_mem in_img = cl_engine.CreateImage2D<CL_UNORM_INT8>(width / 4, height, CL_MEM_READ_WRITE);
	cl_mem out_img = cl_engine.CreateImage2D<CL_FLOAT>(width, height, CL_MEM_READ_WRITE);
	
	{
		TimeProfiler t("WriteImage2D");
		cl_engine.WriteImage2D(in_img, data_in, width / 4, height);
	}

	SetKernelArg4(kernel, in_img, out_img, width, height);

	CHECK_CL_ERROR(status);

	size_t localWorkSize[2]  = { 16, 16 };
	size_t globalWorkSize[2] = { ROUND_UP(localWorkSize[0], width/4),
								 ROUND_UP(localWorkSize[1], height) };

	{
		TimeProfiler t("Kernel Execution");
		status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
		clFinish(queue);
	}

	CHECK_CL_ERROR(status);

	{
		TimeProfiler t("ReadImage2D");
		cl_engine.ReadImage2D(out_img, data_out, width, height);
	}

	for (int i = 0; i < 16; i++)
	{
		printf("%f,", data_out[i]);
	}
	printf("\n");

	for (int i = 0; i < 16; i++)
	{
		printf("%f,", data_out[(width-4)*4 + i]);
	}
	printf("\n");


	clReleaseMemObject(in_img);
	clReleaseMemObject(out_img);

	delete[]data_in;
	delete[]data_out;

	return 0;
}

int copy_channel_main(int argc, char *argv[])
{
	cl_int status = CL_SUCCESS;
	clEngine cl_engine;
	cl_engine.Init(ENGINE_DEVICE_GPU_INTEL);
	//	cl_engine.PrintDeviceInfos();
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


	cl_kernel kernel = cl_engine.FindKernel("copy_channel");
	if (kernel == 0)
	{
		printf("can not find extend_channel kernel\n");
		return -1;
	}

	printf("0\n");

	const int width = 1280;
	const int height = 720;

	int sz = width*height;
	unsigned char *data_in  = new unsigned char[sz];
	unsigned char *data_out = new unsigned char[sz];
	for (int i = 0; i < sz; i++)
	{
		data_in[i] = (i & 0xff);
	}

	cl_mem in_img  = cl_engine.CreateImage2D<CL_UNORM_INT8>(width / 4, height, CL_MEM_READ_WRITE);
	cl_mem out_img = cl_engine.CreateImage2D<CL_UNORM_INT8>(width / 4, height, CL_MEM_READ_WRITE);

	{
		TimeProfiler t("WriteImage2D");
		cl_engine.WriteImage2D(in_img, data_in, width / 4, height);
	}

	SetKernelArg4(kernel, in_img, out_img, width, height);

	CHECK_CL_ERROR(status);

	size_t localWorkSize[2]  = { 16, 16 };
	size_t globalWorkSize[2] = { ROUND_UP(localWorkSize[0], width / 4),
		ROUND_UP(localWorkSize[1], height) };

	{
		TimeProfiler t("Kernel Execution");
		status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
		clFinish(queue);
	}

	CHECK_CL_ERROR(status);

	{
		TimeProfiler t("ReadImage2D");
		cl_engine.ReadImage2D(out_img, data_out, width/4, height);
	}

	for (int i = 0; i < 16; i++)
	{
		printf("%d,", data_out[i]);
	}
	printf("\n");

	for (int i = 0; i < 16; i++)
	{
		printf("%d,", data_out[(width - 16) + i]);
	}
	printf("\n");


	clReleaseMemObject(in_img);
	clReleaseMemObject(out_img);

	delete[]data_in;
	delete[]data_out;

	return 0;
}
