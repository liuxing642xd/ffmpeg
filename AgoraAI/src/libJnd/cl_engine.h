#ifndef __OPENCL_ENGINE_H__
#define __OPENCL_ENGINE_H__

#define __CL_OPT__

#include <cstring>
#include<string>
#include<vector>
#include<map>

#if __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

using namespace std;

#define ROUND_UP(a, b) \
	(((b) + (a) - 1)/(a)*(a))

#define CL_ERROR_STRING(err) \
	case err: \
    return #err; \

extern const char *cluError(cl_int err);
extern void CHECK_CL_ERROR(cl_int status);


enum ENGINE_DEVICE_TYPE
{
	ENGINE_DEVICE_DEFAULT = 0,
	ENGINE_DEVICE_GPU_NVIDIA,
	ENGINE_DEVICE_GPU_AMD,
	ENGINE_DEVICE_GPU_MALI,
	ENGINE_DEVICE_CPU_AMD,
	ENGINE_DEVICE_CPU_INTEL,
	ENGINE_DEVICE_GPU_INTEL,
	ENGINE_DEVICE_GPU_QUALCOMM,
	ENGINE_DEVICE_NUMBER
};


#define ONE_INFO_ITEM(para, type) \
	clDeviceInfoItem(para, #para, type) \

enum PARA_TYPE
{
	P_UINT = 0,
	P_SIZE_T,
	P_SIZE_T3,
	P_ULONG,
	P_BOOL,
	P_CHARS
};

struct clDeviceInfoItem
{
	cl_device_info _para;
	char          _name[128];
	PARA_TYPE      _type;
	union 
	{
		cl_uint        _uint;
		size_t         _size_t;
		size_t         _size_t3[3];
		cl_ulong       _ulong;
		cl_bool        _bool;
		char           _chars[512];
	}_val;


	clDeviceInfoItem(cl_device_info para, const char* name, PARA_TYPE type) 
	{
		_para = para;
		strcpy(_name, name);
		_type = type;
		memset(_val._chars, 0, 512); 
	}

	bool Probe(cl_device_id device)
	{
		cl_int status = CL_SUCCESS;
		switch(_type)
		{
		case P_UINT:
			status = clGetDeviceInfo(device, _para, sizeof(_val._uint), &_val._uint, NULL);
			break;
		case P_SIZE_T:
			status = clGetDeviceInfo(device, _para, sizeof(_val._size_t), &_val._size_t, NULL);
			break;
		case P_SIZE_T3:
			status = clGetDeviceInfo(device, _para, sizeof(_val._size_t3), &_val._size_t3, NULL);
			break;
		case P_ULONG:
			status = clGetDeviceInfo(device, _para, sizeof(_val._ulong), &_val._ulong, NULL);
			break;
		case P_BOOL:
			status = clGetDeviceInfo(device, _para, sizeof(_val._bool), &_val._bool, NULL);
			break;
		case P_CHARS:
			status = clGetDeviceInfo(device, _para, sizeof(_val._chars), &_val._chars, NULL);
			break;
		default:
			return false;
		}

		return status == CL_SUCCESS;
			
	}

	bool Print()
	{
		switch(_type)
		{
		case P_UINT:
			printf("%s: %d\n", _name, _val._uint);
			break;
		case P_SIZE_T:
			printf("%s: %zu\n", _name, _val._size_t);
			break;
		case P_ULONG:
			printf("%s: %llu\n", _name, _val._ulong);
			break;
		case P_BOOL:
			printf("%s: %d\n", _name, _val._bool);
			break;
		case P_SIZE_T3:
			printf("%s: %zu, %zu, %zu\n", _name, _val._size_t3[0], _val._size_t3[1], _val._size_t3[2]);
			break;
		case P_CHARS:
			printf("%s: %s\n", _name, _val._chars);
			break;
		default:
			return false;
		}

		return true;
		
	}

};


class clEngine
{
public:
	clEngine()
	{
		_error_str = "";
		_dev_type  = ENGINE_DEVICE_DEFAULT;
		_platform = 0;
		_queue    = 0;
		_device   = 0;
		_context  = 0;
		_program  = 0;

		_build_options = "";
		_b_support_fp16 = false;
		_b_support_cl20 = false;
		_b_support_svm_fine_grain = false;
		_b_init = false;

	}
	
	~clEngine() 
	{ 
		Release(); 
	}


	bool Init(int type=ENGINE_DEVICE_DEFAULT, bool bProfiling = false);
	bool Release();
	bool CompileSource(vector<string>files, string binfile="");
	bool CompileSource(const char *source, string binfile = "");
	//bool CompileSource(string path, string postfix, string binfile = "");
	bool LoadBin(string binfile);
	void PrintDeviceInfos();
	cl_device_id GetDevice() { return _device; }
	size_t *GetMaxWorkItemSize() { return _max_work_item_size;  }

	string GetErrorStr() { return _error_str; }

	cl_kernel  FindKernel(string name);
	cl_context GetContext() { return _context; }
	cl_command_queue   GetQueue()   { return _queue; }

	string GetDeviceName() { return _device_name;  }
	string GetDeviceVersion() { return _device_version;  }
	bool IsSupportFP16() { return _b_support_fp16;  }
	bool IsSupportCL20() { return _b_support_cl20;  }
	bool IsSupportSVMFineGrain() { return _b_support_svm_fine_grain;  }
	ENGINE_DEVICE_TYPE GetDeviceType() { return _dev_type;  }

	template<unsigned int TYPE> cl_mem CreateImage2D(int w, int h, cl_mem_flags mem_flag= CL_MEM_READ_WRITE, void *host_ptr = 0)
	{
		cl_image_desc desc;
		memset(&desc, 0, sizeof(desc));
		desc.image_type = CL_MEM_OBJECT_IMAGE2D;
		desc.image_width = w;
		desc.image_height = h;

		cl_image_format fmt = { CL_RGBA, TYPE };

		cl_int status;

		cl_mem mem_image = clCreateImage(_context, mem_flag, &fmt, &desc, host_ptr, &status);

		CHECK_CL_ERROR(status);

		return mem_image;
	}

	cl_mem CreateBuffer(size_t size, cl_mem_flags flags, void *host_ptr = 0);

	bool ReadImage2D(cl_mem image, void *host_ptr, int w, int h, bool bmap = false, bool b_block = true);
	bool WriteImage2D(cl_mem image, void *host_ptr, int w, int h, bool bmap = false, bool b_block = true);

	bool ReadBuffer(cl_mem buf, void *host_ptr, int size, bool bmap = false, bool b_block = true);
	bool WriteBuffer(cl_mem buf, void *host_ptr, int size, bool bmap = false, bool b_block = true);

private:
	void ReleaseKernels();
	void SetDeviceType(ENGINE_DEVICE_TYPE type) { _dev_type = type; }

	bool CompileSourceCore(int n, const char **sources, string binfile);

private:

	bool                   _b_init;
	string                 _error_str;
	ENGINE_DEVICE_TYPE     _dev_type;

	cl_platform_id         _platform;
	cl_command_queue       _queue;
	cl_device_id           _device;
	cl_context             _context;
	cl_program             _program;

	string                 _device_name;
	string                 _device_version;
	string                 _build_options;
	bool                    _b_support_fp16;
	bool                    _b_support_cl20;
	bool                    _b_support_svm_fine_grain;
	vector<cl_kernel>      _kernels;
	map<string, cl_kernel> _kernel_map;

	size_t _max_work_item_size[3];
	

	static clDeviceInfoItem  _device_infos[];

};



#endif
