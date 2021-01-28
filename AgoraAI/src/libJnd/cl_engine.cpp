#include "cl_engine.h"
#include "core/utils.h"


#ifndef MAX_PATH
#define MAX_PATH 512
#endif

#include <iostream>

const char *cluError(cl_int err)
{
	switch (err)
	{
		CL_ERROR_STRING(CL_SUCCESS)
			CL_ERROR_STRING(CL_DEVICE_NOT_FOUND)
			CL_ERROR_STRING(CL_DEVICE_NOT_AVAILABLE)
			CL_ERROR_STRING(CL_COMPILER_NOT_AVAILABLE)
			CL_ERROR_STRING(CL_MEM_OBJECT_ALLOCATION_FAILURE)
			CL_ERROR_STRING(CL_OUT_OF_RESOURCES)
			CL_ERROR_STRING(CL_OUT_OF_HOST_MEMORY)
			CL_ERROR_STRING(CL_PROFILING_INFO_NOT_AVAILABLE)
			CL_ERROR_STRING(CL_MEM_COPY_OVERLAP)
			CL_ERROR_STRING(CL_IMAGE_FORMAT_MISMATCH)
			CL_ERROR_STRING(CL_IMAGE_FORMAT_NOT_SUPPORTED)
			CL_ERROR_STRING(CL_BUILD_PROGRAM_FAILURE)
			CL_ERROR_STRING(CL_MAP_FAILURE)
			CL_ERROR_STRING(CL_MISALIGNED_SUB_BUFFER_OFFSET)
			CL_ERROR_STRING(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
			CL_ERROR_STRING(CL_COMPILE_PROGRAM_FAILURE)
			CL_ERROR_STRING(CL_LINKER_NOT_AVAILABLE)
			CL_ERROR_STRING(CL_LINK_PROGRAM_FAILURE)
			CL_ERROR_STRING(CL_DEVICE_PARTITION_FAILED)
			CL_ERROR_STRING(CL_KERNEL_ARG_INFO_NOT_AVAILABLE)
			CL_ERROR_STRING(CL_INVALID_VALUE)
			CL_ERROR_STRING(CL_INVALID_DEVICE_TYPE)
			CL_ERROR_STRING(CL_INVALID_PLATFORM)
			CL_ERROR_STRING(CL_INVALID_DEVICE)
			CL_ERROR_STRING(CL_INVALID_CONTEXT)
			CL_ERROR_STRING(CL_INVALID_QUEUE_PROPERTIES)
			CL_ERROR_STRING(CL_INVALID_COMMAND_QUEUE)
			CL_ERROR_STRING(CL_INVALID_HOST_PTR)
			CL_ERROR_STRING(CL_INVALID_MEM_OBJECT)
			CL_ERROR_STRING(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
			CL_ERROR_STRING(CL_INVALID_IMAGE_SIZE)
			CL_ERROR_STRING(CL_INVALID_SAMPLER)
			CL_ERROR_STRING(CL_INVALID_BINARY)
			CL_ERROR_STRING(CL_INVALID_BUILD_OPTIONS)
			CL_ERROR_STRING(CL_INVALID_PROGRAM)
			CL_ERROR_STRING(CL_INVALID_PROGRAM_EXECUTABLE)
			CL_ERROR_STRING(CL_INVALID_KERNEL_NAME)
			CL_ERROR_STRING(CL_INVALID_KERNEL_DEFINITION)
			CL_ERROR_STRING(CL_INVALID_KERNEL)
			CL_ERROR_STRING(CL_INVALID_ARG_INDEX)
			CL_ERROR_STRING(CL_INVALID_ARG_VALUE)
			CL_ERROR_STRING(CL_INVALID_ARG_SIZE)
			CL_ERROR_STRING(CL_INVALID_KERNEL_ARGS)
			CL_ERROR_STRING(CL_INVALID_WORK_DIMENSION)
			CL_ERROR_STRING(CL_INVALID_WORK_GROUP_SIZE)
			CL_ERROR_STRING(CL_INVALID_WORK_ITEM_SIZE)
			CL_ERROR_STRING(CL_INVALID_GLOBAL_OFFSET)
			CL_ERROR_STRING(CL_INVALID_EVENT_WAIT_LIST)
			CL_ERROR_STRING(CL_INVALID_EVENT)
			CL_ERROR_STRING(CL_INVALID_OPERATION)
			CL_ERROR_STRING(CL_INVALID_GL_OBJECT)
			CL_ERROR_STRING(CL_INVALID_BUFFER_SIZE)
			CL_ERROR_STRING(CL_INVALID_MIP_LEVEL)
			CL_ERROR_STRING(CL_INVALID_GLOBAL_WORK_SIZE)
			CL_ERROR_STRING(CL_INVALID_PROPERTY)
			CL_ERROR_STRING(CL_INVALID_IMAGE_DESCRIPTOR)
			CL_ERROR_STRING(CL_INVALID_COMPILER_OPTIONS)
			CL_ERROR_STRING(CL_INVALID_LINKER_OPTIONS)
			CL_ERROR_STRING(CL_INVALID_DEVICE_PARTITION_COUNT)
#if __APPLE__
#else
			CL_ERROR_STRING(CL_INVALID_PIPE_SIZE)
			CL_ERROR_STRING(CL_INVALID_DEVICE_QUEUE)
#endif
	default:
		return "Unknown CL Error";
	}
}

void CHECK_CL_ERROR(cl_int status)
{
	if (status != CL_SUCCESS)
	{
		printf("%s\n", cluError(status));
	}
}


clDeviceInfoItem  clEngine::_device_infos[] = 
{
	ONE_INFO_ITEM(CL_DEVICE_PROFILE, P_CHARS),
	ONE_INFO_ITEM(CL_DEVICE_VERSION, P_CHARS),
	ONE_INFO_ITEM(CL_DEVICE_NAME, P_CHARS),
	ONE_INFO_ITEM(CL_DEVICE_VENDOR, P_CHARS),
	ONE_INFO_ITEM(CL_DRIVER_VERSION, P_CHARS),
	ONE_INFO_ITEM(CL_DEVICE_PLATFORM, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_TYPE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_OPENCL_C_VERSION, P_CHARS),
	ONE_INFO_ITEM(CL_DEVICE_VENDOR_ID, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MAX_COMPUTE_UNITS, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MAX_WORK_GROUP_SIZE, P_SIZE_T),
	ONE_INFO_ITEM(CL_DEVICE_MAX_WORK_ITEM_SIZES, P_SIZE_T3),
	ONE_INFO_ITEM(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MAX_CLOCK_FREQUENCY, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_ADDRESS_BITS, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MAX_READ_IMAGE_ARGS, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MAX_WRITE_IMAGE_ARGS, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MAX_MEM_ALLOC_SIZE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_IMAGE2D_MAX_WIDTH, P_SIZE_T),
	ONE_INFO_ITEM(CL_DEVICE_IMAGE2D_MAX_HEIGHT, P_SIZE_T),
	ONE_INFO_ITEM(CL_DEVICE_IMAGE3D_MAX_DEPTH, P_SIZE_T),
	ONE_INFO_ITEM(CL_DEVICE_IMAGE_SUPPORT, P_BOOL),
	ONE_INFO_ITEM(CL_DEVICE_MAX_PARAMETER_SIZE, P_SIZE_T),
	ONE_INFO_ITEM(CL_DEVICE_MAX_SAMPLERS, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MEM_BASE_ADDR_ALIGN, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_SINGLE_FP_CONFIG, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_GLOBAL_MEM_SIZE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_MAX_CONSTANT_ARGS, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_LOCAL_MEM_TYPE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_LOCAL_MEM_SIZE, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_ERROR_CORRECTION_SUPPORT, P_BOOL),
	ONE_INFO_ITEM(CL_DEVICE_PROFILING_TIMER_RESOLUTION, P_SIZE_T),
	ONE_INFO_ITEM(CL_DEVICE_ENDIAN_LITTLE, P_BOOL),
	ONE_INFO_ITEM(CL_DEVICE_AVAILABLE, P_BOOL),
	ONE_INFO_ITEM(CL_DEVICE_COMPILER_AVAILABLE, P_BOOL),
	ONE_INFO_ITEM(CL_DEVICE_EXECUTION_CAPABILITIES, P_ULONG),
	ONE_INFO_ITEM(CL_DEVICE_QUEUE_PROPERTIES, P_ULONG),	
	ONE_INFO_ITEM(CL_DEVICE_HOST_UNIFIED_MEMORY, P_BOOL),
	ONE_INFO_ITEM(CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, P_UINT),
	ONE_INFO_ITEM(CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, P_UINT),
	
	ONE_INFO_ITEM(CL_DEVICE_EXTENSIONS, P_CHARS),
};

#define MAX_NUM_PLATFORMS 6

static cl_device_type g_cl_device_type[ENGINE_DEVICE_NUMBER] = { CL_DEVICE_TYPE_DEFAULT, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_GPU,
																 CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_GPU };
static const char *g_device_vendor_name[ENGINE_DEVICE_NUMBER] = { "UNKONW", "NVIDIA", "ADVANCED MICRO DEVICES", "ARM", "ADVANCED MICRO DEVICES", "INTEL", "INTEL", "QUALCOMM" };

bool clEngine::Init(int type, bool bProfiling)
{
	_b_init = false;

	_dev_type = (ENGINE_DEVICE_TYPE)type;
	cl_uint numPlatforms;
    cl_platform_id platforms[MAX_NUM_PLATFORMS];
    cl_int status;
	
	{
		TimeProfiler t("--clGetPlatformIDs 0");
		status = clGetPlatformIDs(0, NULL, &numPlatforms);
	}
	if (numPlatforms > MAX_NUM_PLATFORMS) numPlatforms = MAX_NUM_PLATFORMS;
	if(status != CL_SUCCESS || numPlatforms<=0) 
	{
		_error_str = "No available platforms found!";
		return false;	
	}

	{
		TimeProfiler t("--clGetPlatformIDs 1");
		status = clGetPlatformIDs(numPlatforms, platforms, &numPlatforms);
	}
	if(status != CL_SUCCESS || numPlatforms<=0) 
	{
		_error_str = "Get platformID error!";
		return false;	
	}

	vector<ENGINE_DEVICE_TYPE> prefer_device_type;
	cl_device_type type_device;
	if (_dev_type != ENGINE_DEVICE_DEFAULT)
	{
		prefer_device_type.push_back(_dev_type);
		type_device = g_cl_device_type[_dev_type];
	}
	else
	{
		prefer_device_type.push_back(ENGINE_DEVICE_GPU_NVIDIA);
		prefer_device_type.push_back(ENGINE_DEVICE_GPU_INTEL);
		prefer_device_type.push_back(ENGINE_DEVICE_GPU_AMD);
		prefer_device_type.push_back(ENGINE_DEVICE_GPU_QUALCOMM);
		prefer_device_type.push_back(ENGINE_DEVICE_GPU_MALI);
		type_device = CL_DEVICE_TYPE_GPU;
	}

	_platform = 0;
	_device = 0;

	
	char pbuf[256];
	{
		TimeProfiler t("--Find prefer device");
		for (int k = 0; k < prefer_device_type.size(); k++)
		{
			for (int i = 0; i < numPlatforms; i++)
			{
				status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pbuf), pbuf, NULL);
		

				if (status != CL_SUCCESS)
				{
					_error_str = "Get platformInfo error!";
					return false;
				}

				string vendors = pbuf;

				for (string::size_type ii = 0; ii != vendors.size(); ++ii)
				{
					if (pbuf[ii] >= 'a' && pbuf[ii] <= 'z')
					{
						pbuf[ii] = 'A' + pbuf[ii] - 'a';
					}
				}

				string upcase_vendors = pbuf;				
				int nPos = upcase_vendors.find(g_device_vendor_name[prefer_device_type[k]], 0);

				cout << "vendors: " << upcase_vendors << endl;

				bool bApple = false;
				if (nPos == -1)
				{
					nPos = upcase_vendors.find("APPLE", 0);
					if (nPos != -1)
					{
						bApple = true;
					}
				}
				
				if(nPos != -1)
				{
					_platform = platforms[i];
					status = clGetDeviceIDs(_platform, type_device, 1, &_device, NULL);

					if (status == CL_SUCCESS && _device)
					{
						_dev_type = bApple ? ENGINE_DEVICE_DEFAULT : prefer_device_type[k];
						std::cout << "OpenCL Platform:" << vendors << endl;
						break;
					}
					else
					{
						_platform = 0;
						_device = 0;

						std::cout << "clGetDviceIDs error:" << upcase_vendors << endl;
					}
				}


			}

			if (_platform && _device)
			{
				break;
			}
		}
	}

	if(_platform==0) 
	{
		_error_str = "Can not detect OpenCL platform!";
		return false;
	}

	if (_device == 0)
	{
		_error_str = "Get deviceIds error!";
		return false;
	}

	{
		status = clGetDeviceInfo(_device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(_max_work_item_size), _max_work_item_size, NULL);
		status = clGetDeviceInfo(_device, CL_DEVICE_NAME, sizeof(pbuf), pbuf, NULL);
		_device_name = pbuf;
		status = clGetDeviceInfo(_device, CL_DEVICE_VERSION, sizeof(pbuf), pbuf, NULL);
		_device_version = pbuf;
		if (_device_version.find("OpenCL 2.") != -1)
		{
			_b_support_cl20 = true;
			cout << "Device Support OpenCL 2.0 or obove !" << endl;
		}
		else
		{
			_b_support_cl20 = false;
			cout << "Device Not Support OpenCL 2.0 or obove !" << endl;
		}

#if __APPLE__
    //mac not support opencl 2.x
#else
		if (_b_support_cl20)
		{
			cl_device_svm_capabilities svm_cap;
			status = clGetDeviceInfo(_device, CL_DEVICE_SVM_CAPABILITIES, sizeof(svm_cap), &svm_cap, NULL);
			if (svm_cap&CL_DEVICE_SVM_FINE_GRAIN_BUFFER)
			{
				_b_support_svm_fine_grain = true;
				cout << "Device Support SVM_FINE_GRAIN_BUFFER !" << endl;
			}
			else
			{
				_b_support_svm_fine_grain = false;
				cout << "Device Not Support SVM_FINE_GRAIN_BUFFER !" << endl;
			}
		}
#endif

		size_t ret_size;
		status = clGetDeviceInfo(_device, CL_DEVICE_EXTENSIONS, 0, NULL, &ret_size);
		char *temp_buf = new char[ret_size];
		status = clGetDeviceInfo(_device, CL_DEVICE_EXTENSIONS, ret_size, temp_buf, NULL);
		string temp = temp_buf;
		delete[]temp_buf;
		if (temp.find("cl_khr_fp16") != -1 && _b_support_cl20)
		{
			_b_support_fp16 = true;
			cout << "Device Support fp16 !" << endl;
		}
		else
		{
			_b_support_fp16 = false;
			cout << "Device Not Support fp16 !" << endl;
		}
	}
	
		
	cout << "Device Name:" << _device_name << endl;
	cout << "Device Version:" << _device_version << endl;

	cl_context_properties cps[3] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)_platform,
		0
	};

	{
		TimeProfiler t("--clCreateContext");
		_context = clCreateContext(cps, 1, &_device, NULL, NULL, &status);
	}

	if(!_context) 
	{
		_error_str = "Create Context error";
		return false;
	}

	cl_command_queue_properties properties = bProfiling ? CL_QUEUE_PROFILING_ENABLE : 0;

	_queue = clCreateCommandQueue(_context, _device, properties, &status);

	if(!_queue)
	{
		_error_str = "Create Command Queue error";
		return false;
	}

	int nInfo = sizeof(_device_infos)/sizeof(clDeviceInfoItem);

	for(int i=0; i<nInfo; i++)
	{
		_device_infos[i].Probe(_device);
	}


	_build_options = "";


	if (_b_support_fp16)//_dev_type != ENGINE_DEVICE_GPU_NVIDIA)
	{
		_build_options = "-DHALF_FP";
	}
	if (_dev_type == ENGINE_DEVICE_GPU_QUALCOMM)
	{
		_build_options += " -DQUALCOMM";
	}
	

	_b_init = true;
	return _b_init;
}

void clEngine::PrintDeviceInfos()
{
	static const char* strOrder[] =
	{
		"CL_R",
		"CL_A",
		"CL_RG",
		"CL_RA",
		"CL_RGB",
		"CL_RGBA",
		"CL_BGRA",
		"CL_ARGB",
		"CL_INTENSITY",
		"CL_LUMINANCE",
		"CL_Rx",
		"CL_RGx",
		"CL_RGBx",
		"CL_DEPTH",
		"CL_DEPTH_STENCIL",
	};

	static const char* strDataType[] =
	{
		"CL_SNORM_INT8",
		"CL_SNORM_INT16",
		"CL_UNORM_INT8",
		"CL_UNORM_INT16",
		"CL_UNORM_SHORT_565",
		"CL_UNORM_SHORT_555",
		"CL_UNORM_INT_101010",
		"CL_SIGNED_INT8",
		"CL_SIGNED_INT16",
		"CL_SIGNED_INT32",
		"CL_UNSIGNED_INT8",
		"CL_UNSIGNED_INT16",
		"CL_UNSIGNED_INT32",
		"CL_HALF_FLOAT",
		"CL_FLOAT",
		"CL_UNORM_INT24"
	};

	int nInfo = sizeof(_device_infos)/sizeof(clDeviceInfoItem);

	for(int i=0; i<nInfo; i++)
	{
		_device_infos[i].Print();
	}

	cl_bool imageSupport = CL_FALSE;
	clGetDeviceInfo(_device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
	if (imageSupport != CL_TRUE)
	{
		printf("CL Image not supported\n");	
	}
	else
	{
		cl_uint num_image_formats;

		clGetSupportedImageFormats(_context, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, 0, NULL, &num_image_formats);
		printf("CL Image supported: %d\n", num_image_formats);

		cl_image_format *image_formats = new cl_image_format[num_image_formats];
		clGetSupportedImageFormats(_context, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, num_image_formats, image_formats, &num_image_formats);

		for (int i = 0; i < num_image_formats; i++)
		{
			printf("(%s, %s)\n", strOrder[image_formats[i].image_channel_order - CL_R], strDataType[image_formats[i].image_channel_data_type - CL_SNORM_INT8]);
		}

		delete[]image_formats;
	
	}

}


void clEngine::ReleaseKernels()
{
	int n = _kernels.size();

	for(int i=0; i<n; i++)
	{
		if (_kernels[i])
		{
			clReleaseKernel(_kernels[i]);
			_kernels[i] = 0;
		}
	}

	_kernels.clear();
}

bool clEngine::Release()
{
	if(_queue)
	{
		clReleaseCommandQueue(_queue);
		_queue = 0;
	}

	ReleaseKernels();

	if(_program)
	{
		clReleaseProgram(_program);
		_program = 0;
	}
	
	if(_context)
	{
		clReleaseContext(_context);
		_context = 0;
	}

	return true;

}

cl_kernel clEngine::FindKernel(string name)
{
	
	int n = _kernels.size();

	char sn[256];

	for(int i=0; i<n; i++)
	{
		size_t sz;
		cl_kernel kn = _kernels[i];
		clGetKernelInfo(kn, CL_KERNEL_FUNCTION_NAME, 256, sn, &sz);
		if(name == sn) return kn;
	}
	return 0;
	
//	return _kernel_map[name];	
}

bool clEngine::LoadBin(string binfile)
{
	if( !_b_init ) 
	{
		_error_str = "clEngine not initialized!";
		return false;
	}

	FILE *fp = fopen(binfile.c_str(), "rb");
	if(fp == 0)
	{
		_error_str = "Open file " + binfile + " error!";
		return false;
	}
	
	fseek(fp, 0, SEEK_END);   
	size_t size =   ftell(fp);   

	char* buf = new char[size];
	fseek(fp, 0, SEEK_SET);
	fread(buf, size, 1, fp);
	fclose(fp);

	if(_program)
	{
		clReleaseProgram(_program);
		_program = 0;
	}
	
	cl_int status;

	_program = clCreateProgramWithBinary(_context, 1, &_device, &size, (const unsigned char**)&buf, NULL, &status);

	delete []buf;

	 if(_program == 0)
	 {
		 _error_str = "Create Programe With Binary error!";
		 return false;
	 }
	 else
	 {

		 status = clBuildProgram(_program, 0, NULL, _build_options.c_str(), NULL, NULL);

		 if(status != CL_SUCCESS)
		 {
			 _error_str = "Build Programe Error!";
			 return false;
		 }
		 else
		 {
			cl_uint nKernel;
			status = clCreateKernelsInProgram(_program, 0, NULL, &nKernel);

			if(status == CL_SUCCESS && nKernel>0)
			{
				ReleaseKernels();
				cl_kernel *ks = new cl_kernel[nKernel];
				status = clCreateKernelsInProgram(_program, nKernel, ks, &nKernel);
				if(status == CL_SUCCESS)
				{
					char sn[256];
					for(int i=0; i<nKernel; i++)
					{
						size_t sz;
						_kernels.push_back(ks[i]);
						clGetKernelInfo(ks[i], CL_KERNEL_FUNCTION_NAME, 256, sn, &sz);
						_kernel_map[sn] = ks[i];
					}
					delete []ks;
				}
				else
				{
					 _error_str = "Create Kernels Error!";
					 delete []ks;
					 return false;

				}
				
			}
				
		 }
		 
	 }

	 return true;
}


bool clEngine::CompileSourceCore(int n, const char **sources, string binfile)
{
	cl_int status;
	bool res = true;

	_program = clCreateProgramWithSource(_context, n, sources, NULL, &status);
	
	if (_program == 0)
	{
		_error_str = "Create Programe With Source error!";
		res = false;
	}
	else
	{

		status = clBuildProgram(_program, 0, NULL, _build_options.c_str(), NULL, NULL);


		if (status == CL_SUCCESS)
		{
			cl_uint nKernel;
			status = clCreateKernelsInProgram(_program, 0, NULL, &nKernel);

			if (status == CL_SUCCESS && nKernel>0)
			{
				ReleaseKernels();
				cl_kernel *ks = new cl_kernel[nKernel];
				status = clCreateKernelsInProgram(_program, nKernel, ks, &nKernel);
				if (status == CL_SUCCESS)
				{
					for (int i = 0; i<nKernel; i++)
					{
						_kernels.push_back(ks[i]);
					}
				}
				delete[]ks;
			}

			if (binfile != "")
			{
				size_t len;
				status = clGetProgramInfo(_program, CL_PROGRAM_BINARY_SIZES, sizeof(len), &len, NULL);
				char *buffer = new char[len];

				status = clGetProgramInfo(_program, CL_PROGRAM_BINARIES, sizeof(char *), &buffer, NULL);

				FILE *fp = fopen(binfile.c_str(), "wb");

				if (fp)
				{
					fwrite(buffer, 1, len, fp);
					fclose(fp);
				}
				else
				{
					_error_str = "Save bin to " + binfile + " error!";
					res = false;
				}

				delete[]buffer;
			}
		}
		else
		{
			_error_str = "Build Programe Error!\n";

			size_t len;
			clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
			char *buffer = new char[len + 1];

			clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, len + 1, buffer, &len);

			buffer[len] = '\0';
			_error_str += buffer;

			delete[]buffer;

			res = false;
		}
	}

	return res;
}

bool clEngine::CompileSource(const char *source, string binfile)
{
	int len = strlen(source);

	return CompileSourceCore(1, &source, binfile);
}

bool clEngine::CompileSource(vector<string> files, string binfile)
{
	printf("Start Compiling OpenCL kernels:\n");

	if( !_b_init ) 
	{
		_error_str = "clEngine not initialized!";
		return false;
	}

	cl_int status;
	
	int n = files.size();

	if(n==0) 
	{
		_error_str = "No OpenCL source file!";

		return false;
	}

	char **sources  = new char*[n];
	
	bool res = true;
	int i;
	for(i=0; i<n; i++)
	{
		//printf("%s\n", files[i].c_str());
		FILE *fp = fopen(files[i].c_str(), "rb");
		if(fp == 0)
		{
			res = false;
			_error_str = "Open file " + files[i] + "error!";
			break;
		}
		
		fseek(fp, 0, SEEK_END);   
		int size =   ftell(fp);   

		char* buf = new char[size + 1];
		fseek(fp, 0, SEEK_SET);
		fread(buf, size, 1, fp);
		fclose(fp);

		buf[size] = '\0';

		sources[i] = buf;

	}

	if(i==n)
	{
		res = CompileSourceCore(n, (const char**)sources, binfile);
	}
	
	for(int k=0; k<i; k++)
	{
		delete []sources[k];	
	}
	delete []sources;

	return res;

}



cl_mem clEngine::CreateBuffer(size_t size, cl_mem_flags flags, void *host_ptr)
{
	cl_int status;

	cl_mem mem_buf = clCreateBuffer(_context, flags, size, host_ptr, &status);

	CHECK_CL_ERROR(status);

	return mem_buf;
}

bool clEngine::ReadImage2D(cl_mem image, void *host_ptr, int w, int h, bool bmap, bool b_block)
{
	cl_int status;

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { (size_t)w, (size_t)h, 1 };

	if (bmap)
	{
		size_t image_row_pitch;

		void *  map_ptr = clEnqueueMapImage(_queue, image, CL_TRUE, CL_MAP_READ, origin, region, &image_row_pitch,
			NULL, 0, NULL, NULL, &status);

		size_t ele_sz;

		clGetImageInfo(image, CL_IMAGE_ELEMENT_SIZE, sizeof(ele_sz), &ele_sz, NULL);	

		if (image_row_pitch != w*ele_sz)
		{
			for (int i = 0; i < h; i++)
			{
				memcpy((unsigned char *)host_ptr + i*w*ele_sz, (unsigned char *)map_ptr + i*image_row_pitch, w*ele_sz);
			}
		}
		else
		{
			memcpy(host_ptr, map_ptr, w*h*ele_sz);
		}

		status = clEnqueueUnmapMemObject(_queue, image, map_ptr, 0, NULL, NULL);

	}
	else
	{
		cl_bool blocked = b_block ? CL_TRUE : CL_FALSE;
		status = clEnqueueReadImage(_queue, image, blocked, origin, region, 0, 0, host_ptr, 0, NULL, NULL);
	}

	if (status != CL_SUCCESS)
	{
		printf("%s\n", cluError(status));
		return false;
	}
	return true;
}

bool clEngine::WriteImage2D(cl_mem image, void *host_ptr, int w, int h, bool bmap, bool b_block)
{
	cl_int status;

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { (size_t)w, (size_t)h, 1 };

	if (bmap)
	{
		size_t image_row_pitch;

		void *  map_ptr = clEnqueueMapImage(_queue, image, CL_TRUE, CL_MAP_WRITE_INVALIDATE_REGION, origin, region, &image_row_pitch,
			NULL, 0, NULL, NULL, &status);

		if (image_row_pitch != w * 4)
		{
			for (int i = 0; i < h; i++)
			{
				memcpy((unsigned char *)map_ptr + i*image_row_pitch, (unsigned char *)host_ptr + i*w*4, w*4);
			}
		}
		else
		{
			memcpy(map_ptr, host_ptr, w*h * 4);
		}

		status = clEnqueueUnmapMemObject(_queue, image, map_ptr, 0, NULL, NULL);

	}
	else
	{
		cl_bool blocked = b_block ? CL_TRUE : CL_FALSE;
		status = clEnqueueWriteImage(_queue, image, blocked, origin, region, 0, 0, host_ptr, 0, NULL, NULL);
	}

	if (status != CL_SUCCESS)
	{
		printf("%s\n", cluError(status));
		return false;
	}
	return true;
}

bool  clEngine::ReadBuffer(cl_mem buf, void *host_ptr, int size, bool bmap, bool b_block)
{
	cl_int status;

	if (bmap)
	{
		void *  map_ptr = clEnqueueMapBuffer(_queue, buf, CL_TRUE, CL_MAP_READ, 0, (size_t)(size), 0, NULL, NULL, &status);		
		memcpy(host_ptr, map_ptr, size);		
		status = clEnqueueUnmapMemObject(_queue, buf, map_ptr, 0, NULL, NULL);
	}
	else
	{
		cl_bool blocked = b_block ? CL_TRUE : CL_FALSE;
		status = clEnqueueReadBuffer(_queue, buf, blocked, 0, (size_t)(size),  host_ptr, 0, NULL, NULL);
	}

	if (status != CL_SUCCESS)
	{
		printf("%s\n", cluError(status));
		return false;
	}
	return true;
}


bool clEngine::WriteBuffer(cl_mem buf, void *host_ptr, int size, bool bmap, bool b_block)
{
	cl_int status;

	if (bmap)
	{
		void *  map_ptr = clEnqueueMapBuffer(_queue, buf, CL_TRUE, CL_MAP_WRITE, 0, (size_t)(size), 0, NULL, NULL, &status);
		memcpy(map_ptr, host_ptr, size);
		status = clEnqueueUnmapMemObject(_queue, buf, map_ptr, 0, NULL, NULL);
	}
	else
	{
		cl_bool blocked = b_block ? CL_TRUE : CL_FALSE;
		status = clEnqueueWriteBuffer(_queue, buf, blocked, 0, (size_t)(size), host_ptr, 0, NULL, NULL);
	}

	if (status != CL_SUCCESS)
	{
		printf("%s\n", cluError(status));
		return false;
	}
	return true;
}