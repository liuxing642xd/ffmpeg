
#include "cl_engine.h"
#include "cl_kernel_cfg.h"
#include "jnd_filter_cl.h"
#include "core/utils.h"
#include "half_float.h"
#include <sstream>


extern const char* jnd_kernel_preprocessor_string;
extern const char* jnd_kernel_cl_string;

int jnd_kernel_version = 104;

#define RELEASE_CL_MEM(mem) \
if(mem) \
{ \
	clReleaseMemObject((cl_mem)mem); \
	mem = 0; \
} \


#define NUM_INTERNALE_TUNED_DEVICE 13


static JndGroupCfg g_internal_group_cfg[NUM_INTERNALE_TUNED_DEVICE] =
{
	{ "Intel(R) UHD Graphics 620", "OpenCL 2.1 NEO ", 3,{ { "conv1",{ 8, 12, 1 } },{ "conv2",{ 1, 12, 1 } },{ "conv3",{ 1, 8, 1 } } } },
	{ "Intel(R) HD Graphics 620", "OpenCL 2.1 ", 3,{ { "conv1",{ 192, 1, 1 } },{ "conv2",{ 2, 91, 1 } },{ "conv3",{ 1, 84, 1 } } } },
	{ "GeForce MX250", "OpenCL 1.2 CUDA", 3,{ { "conv1",{ 1, 60, 1 } },{ "conv2",{ 1, 180, 1 } },{ "conv3",{ 1, 240, 1 } } } },
	{ "Mali-G76", "unknwon", 3,{ { "conv1",{ 4, 1, 1 } },{ "conv2",{ 2, 4, 1 } },{ "conv3",{ 2, 2, 1 } } } },
	{ "Mali-G52", "OpenCL 2.0 v1.r16p0-01rel0.b2dcc2721215660818f1957ab34ed417", 3,{ { "conv1",{ 84, 3, 1 } },{ "conv2",{ 48, 8, 1 } },{ "conv3",{ 1, 4, 1 } } } },
	{ "GeForce GTX 1050 Ti", "unkown", 3,{ { "conv1",{ 2, 48, 1 } },{ "conv2",{ 1, 32, 1 } },{ "conv3",{ 1, 414, 1 } } } },
	{ "Intel(R) UHD Graphics 630", "unkown", 3,{ { "conv1",{ 1, 24, 1 } },{ "conv2",{ 1, 228, 1 } },{ "conv3",{ 1, 184, 1 } } } },
	{ "QUALCOMM Adreno(TM)", "OpenCL 2.0 Adreno(TM) 640", 3,{ { "conv1",{ 64, 12, 1 } },{ "conv2",{ 32, 2, 1 } },{ "conv3",{ 33, 2, 1 } } } },
	{ "QUALCOMM Adreno(TM)", "OpenCL 2.0 Adreno(TM) 512", 3,{ { "conv1",{ 40, 24, 1 } },{ "conv2",{ 16, 60, 1 } },{ "conv3",{ 16, 52, 1 } } } },
	{ "QUALCOMM Adreno(TM)", "OpenCL 2.0 Adreno(TM) 506", 3,{ { "conv1",{ 80, 12, 1 } },{ "conv2",{ 16, 60, 1 } },{ "conv3",{ 16, 8, 1 } } } },
	{ "Mali-G71", "OpenCL 1.2 v1.r5p0-01rel0.3d0f70caac75835c7499d0b80a74ae47", 3,{ { "conv1",{ 2, 10, 1 } },{ "conv2",{ 2, 2, 1 } },{ "conv3",{ 160, 2, 1 } } } },
	{ "Mali-G52", "OpenCL 2.0 v1.r16p0-01rel0.b2dcc2721215660818f1957ab34ed417", 3,{ { "conv1",{ 50, 5, 1 } },{ "conv2",{ 16, 18, 1 } },{ "conv3",{ 1, 4, 1 } } } },
	{ "gfx902", "OpenCL 1.2 AMD-APP (2906.10)", 3,{ { "conv1",{ 2, 64, 1 } },{ "conv2",{ 1, 256, 1 } },{ "conv3",{ 1, 52, 1 } } } }
};


bool JndFilterCL::dump_kernel_group_cfg()
{
	printf("{\"%s\", \"%s\", %d, {", _cl_engine->GetDeviceName().c_str(), _cl_engine->GetDeviceVersion().c_str(), 3);
	for (int i = 0; i < 3; i++)
	{
		const size_t *local = _kernel_cfg[i]->get_local_work_size();
		printf("{\"%s\", {%zu, %zu, %zu} }", _kernel_cfg[i]->get_op_name().c_str(), local[0], local[1], local[2]);
		if (i < 2)
		{
			printf(",");
		}
	}
	printf("}};\n");

	return true;
}

bool JndFilterCL::load_kernel_group_cfg(JndGroupCfg *cfg, int num)
{
	string device_name = _cl_engine->GetDeviceName();
	string device_version = _cl_engine->GetDeviceVersion();
	ENGINE_DEVICE_TYPE device_type = _cl_engine->GetDeviceType();
	for (int i = 0; i < num; i++)
	{
		if (cfg[i]._num_layers == 3 && 
			( (device_type != ENGINE_DEVICE_GPU_QUALCOMM && cfg[i]._device_name == device_name) 
				|| (device_type == ENGINE_DEVICE_GPU_QUALCOMM && cfg[i]._device_version == device_version)) )		
		{
			int a = 0;
			for (int j = 0; j < 3; j++)
			{
				string op_name = _kernel_cfg[j]->get_op_name();

				if (cfg[i]._layer_cfg[j]._op_name == op_name)
				{
					_kernel_cfg[j]->set_local_work_size(cfg[i]._layer_cfg[j]._group_size);
					a++;
				}
			}

			if (a == 3)
			{
				return true;
			}
		}
	}

	return false;
}

JndFilterCL::JndFilterCL()
{
	for (int i = 0; i < 3; i++)
	{
		_conv_weight_img[i] = 0;
	}

	for (int i = 0; i < 3; i++)
	{
		_kernel_cfg[i] = new clKernelCfg;
	}

	for (int i = 0; i < 4; i++)
	{
		_conv_inout_img[i] = 0;
	}

	
	_temp_buf = 0;
	_width  = 0;
	_height = 0;
	_stride = 0;

	_b_mapping   = true;
	_b_profiling = false;
	_b_tuning    = false;


	_cl_engine = new clEngine;
	
}

JndFilterCL::~JndFilterCL()
{
	release();

	for (int i = 0; i < 3; i++)
	{	
		delete _kernel_cfg[i];
	}

	delete _cl_engine;
}

static void conv_float_2_half_array(float *in, cl_half *out, int num)
{
	for (int i = 0; i < num; i++)
	{
		out[i] = to_half(in[i]);
	}
}

bool JndFilterCL::init(JND_NET_PARAS net_paras, string cl_bin_file, int run_mode, int prefer_dev_type, vector<JndGroupCfg> external_group_cfg)
{
	_b_init = false;

	printf("cl_bin_file:%s, run_mode:%d, prefer_dev_type:%d\n", cl_bin_file.c_str(), run_mode, prefer_dev_type);

	bool res = true;
	cl_int status = CL_SUCCESS;
		
	if (run_mode == 1)
	{
		_b_profiling = true;
		_b_tuning    = false;
	}
	else if (run_mode == 2)
	{
		_b_profiling = false;
		_b_tuning    = true;
	}
	else
	{
		_b_profiling = false;
		_b_tuning    = false;
	}

	_external_group_cfg = external_group_cfg;

	{
		TimeProfiler t("--_cl_engine.Init");	
		if (prefer_dev_type >= ENGINE_DEVICE_NUMBER) prefer_dev_type = 0;
		res = _cl_engine->Init(ENGINE_DEVICE_TYPE(prefer_dev_type), _b_profiling || _b_tuning);
	}
	
	if (!res)
	{
		_error_str = _cl_engine->GetErrorStr();
		std::cout << _error_str << endl;
		return false;
	}

//	_cl_engine->PrintDeviceInfos();

	vector<string> files;
#ifdef PLATFORM_X86
	files.push_back("../Test/Test.cl");
#else
	files.push_back("Test.cl");
#endif
	{
		TimeProfiler t("--_cl_engine->CompileSource");

		stringstream ss;
		ss << jnd_kernel_version;
		string temp;
		ss >> temp;

		string version_kernel_name = "jnd_kernel_version_" + temp;
		string jnd_kernel_version_string = "__kernel void ";
		jnd_kernel_version_string += version_kernel_name;
		jnd_kernel_version_string += "(__global float *a) { } \r\n";
		
		string whole_kernel_string = jnd_kernel_preprocessor_string;

		whole_kernel_string += jnd_kernel_cl_string;
		whole_kernel_string += jnd_kernel_version_string;
		
		//res = _cl_engine->CompileSource(files);
		//printf("%s\n", jnd_kernel_cl_string);
		if (cl_bin_file != "")
		{
			int pos = cl_bin_file.length();
			cl_bin_file = cl_bin_file.substr(0, pos - 4);
			cl_bin_file += "_";
			cl_bin_file += _cl_engine->GetDeviceName();
			cl_bin_file += ".bin";

			res = _cl_engine->LoadBin(cl_bin_file);
			if (res)
			{
				cl_kernel kernel = _cl_engine->FindKernel(version_kernel_name);
				if (kernel == 0)
				{
					printf("Jnd Kernel need updated \n");
					res = false;
				}
				else
				{
					printf("LoadCLKernel bin file successfully !\n");		
				}

			}

			if(!res)
			{
				res = _cl_engine->CompileSource(whole_kernel_string.c_str(), cl_bin_file);
			}

		}
		else
		{
			res = _cl_engine->CompileSource(whole_kernel_string.c_str(), cl_bin_file);
		}
	}
	if (!res)
	{
		_error_str = _cl_engine->GetErrorStr();
		std::cout << _error_str << endl;
		return false;
	}


	const char*kernel_name[3] = { "jnd_conv1_3x3_s1x1_n4_c1", "jnd_conv2_3x3_s1x1_n4_c4", "jnd_conv3_3x3_s1x1_n1_c4"};
	const char*op_name[3]     = { "conv1", "conv2", "conv3"};
	
	for (int i = 0; i < 3; i++)
	{
		_kernel_cfg[i]->init(_cl_engine, kernel_name[i], op_name[i]);	
	}

	update_net_paras(net_paras);

	_b_init = res;

	return res;
}

bool JndFilterCL::update_net_paras(JND_NET_PARAS net_paras)
{

	float temp_weight_bias1[36 + 4];
	float temp_weight_bias2[144 + 4];
	float temp_weight_bias3[36 + 4];
	{
		TimeProfiler t("nchw_2_hwcn");
		nchw_2_hwcn(net_paras.conv1_weight, temp_weight_bias1, 4, 3, 3, 1);
		memcpy(temp_weight_bias1 + 36, net_paras.conv1_bias, 4 * sizeof(float));
		nchw_2_hwcn(net_paras.conv2_weight, temp_weight_bias2, 4, 3, 3, 4);
		memcpy(temp_weight_bias2 + 144, net_paras.conv2_bias, 4 * sizeof(float));
		nchw_2_hwcn(net_paras.conv3_weight, temp_weight_bias3, 1, 3, 3, 4);
		for (int i = 0; i < 4; i++)
		{
			temp_weight_bias3[36 + i] = net_paras.conv3_bias[0];
		}
	}

	//_conv_weight_img[0] = _cl_engine->CreateFloat32Image2D(10, 1);
	//_conv_weight_img[1] = _cl_engine->CreateFloat32Image2D(37, 1);
	//_conv_weight_img[2] = _cl_engine->CreateFloat32Image2D(10, 1);

	//{
	//	TimeProfiler t("Write Weight Image");
	//	_cl_engine->WriteImage2D(_conv_weight_img[0], temp_weight_bias1, 10, 1);
	//	_cl_engine->WriteImage2D(_conv_weight_img[1], temp_weight_bias2, 37, 1);
	//	_cl_engine->WriteImage2D(_conv_weight_img[2], temp_weight_bias3, 10, 1);
	//}

	cl_half temp_weight_bias1_half[36 + 4];
	cl_half temp_weight_bias2_half[144 + 4];
	cl_half temp_weight_bias3_half[36 + 4];

	{
		TimeProfiler t("float_2_half");
		conv_float_2_half_array(temp_weight_bias1, temp_weight_bias1_half, 36 + 4);
		conv_float_2_half_array(temp_weight_bias2, temp_weight_bias2_half, 144 + 4);
		conv_float_2_half_array(temp_weight_bias3, temp_weight_bias3_half, 36 + 4);
	}

	//_conv_weight_img[0] = _cl_engine->CreateFloat16Image2D(10, 1);
	//_conv_weight_img[1] = _cl_engine->CreateFloat16Image2D(37, 1);	
	//_conv_weight_img[2] = _cl_engine->CreateFloat16Image2D(10, 1);
	//{
	//	TimeProfiler t("Write Weight Image");
	//	_cl_engine->WriteImage2D((cl_mem)_conv_weight_img[0], temp_weight_bias1_half, 10, 1);
	//	_cl_engine->WriteImage2D((cl_mem)_conv_weight_img[1], temp_weight_bias2_half, 37, 1);
	//	_cl_engine->WriteImage2D((cl_mem)_conv_weight_img[2], temp_weight_bias3_half, 10, 1);
	//}

	size_t buf_sizes[3];
	void  *temp_ptrs[3];
	
	cl_mem_flags constant_buf_flags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
	
	if (_cl_engine->IsSupportFP16())
	{
		buf_sizes[0] = 40 * sizeof(cl_half);
		buf_sizes[1] = 148 * sizeof(cl_half);
		buf_sizes[2] = 40 * sizeof(cl_half);
		temp_ptrs[0] = temp_weight_bias1_half;
		temp_ptrs[1] = temp_weight_bias2_half;
		temp_ptrs[2] = temp_weight_bias3_half;
	}
	else
	{
		buf_sizes[0] = 40 * sizeof(float);
		buf_sizes[1] = 148 * sizeof(float);
		buf_sizes[2] = 40 * sizeof(float);
		temp_ptrs[0] = temp_weight_bias1;
		temp_ptrs[1] = temp_weight_bias2;
		temp_ptrs[2] = temp_weight_bias3;
	}

	
	for (int i = 0; i < 3; i++)
	{
		if (_conv_weight_img[i])
		{
			_cl_engine->WriteBuffer((cl_mem)(_conv_weight_img[i]), temp_ptrs[i], buf_sizes[i], _b_mapping);
		}
		else
		{
			_conv_weight_img[i] = _cl_engine->CreateBuffer(buf_sizes[i], constant_buf_flags, temp_ptrs[i]);
		}
	}

	
	return true;
}


bool JndFilterCL::release()
{
	for (int i = 0; i < 3; i++)
	{
		RELEASE_CL_MEM(_conv_weight_img[i]);
	}

	for (int i = 0; i < 4; i++)
	{
		RELEASE_CL_MEM(_conv_inout_img[i]);
	}

	_cl_engine->Release();

	if (_temp_buf)
	{
		delete[]_temp_buf;
		_temp_buf = 0;
	}
	_width = 0;
	_height = 0;
	_stride = 0;


	return true;
}

bool JndFilterCL::prepare(int w, int h, int stride, unsigned char *in, unsigned char *out)
{
	for (int i = 0; i < 4; i++)
	{
		RELEASE_CL_MEM(_conv_inout_img[i]);
	}

	//if (_temp_buf)
	//{
	//	delete[]_temp_buf;
	//	_temp_buf = 0;
	//}
	//_temp_buf = new float[w*h * 4];

	_width = w;
	_height = h;
	_stride = stride;

	//_conv_inout_img[0] = _cl_engine->CreateImage2D<CL_UNORM_INT8>(stride / 4, h, _b_mapping ? CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR : CL_MEM_READ_ONLY, _b_mapping ? in : 0);
	_conv_inout_img[0] = _cl_engine->CreateImage2D<CL_UNORM_INT8>(stride / 4, h, _b_mapping ? CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR : CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY);
	//_conv_inout_img[0] = _cl_engine->CreateImage2D<CL_FLOAT>(w / 4, h, _b_mapping ? CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR : CL_MEM_READ_WRITE);
	_conv_inout_img[1] = _cl_engine->CreateImage2D<CL_HALF_FLOAT>(w, h, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS);
	_conv_inout_img[2] = _cl_engine->CreateImage2D<CL_HALF_FLOAT>(w, h, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS);
	//_conv_inout_img[3] = _cl_engine->CreateImage2D<CL_UNORM_INT8>(stride / 4, h, _b_mapping ? CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR : CL_MEM_WRITE_ONLY, _b_mapping ? out : 0);
	_conv_inout_img[3] = _cl_engine->CreateImage2D<CL_UNORM_INT8>(stride / 4, h, _b_mapping ? CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR : CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY);
	//_conv_inout_img[3] = _cl_engine->CreateBuffer(w*h, _b_mapping ? CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR : CL_MEM_WRITE_ONLY);

	for (int i = 0; i < 3; i++)
	{
		_kernel_cfg[i]->set_arg(0, &_conv_inout_img[i], sizeof(_conv_inout_img[i]));
		_kernel_cfg[i]->set_arg(1, &_conv_inout_img[(i + 1)], sizeof(_conv_inout_img[(i + 1)]));
		_kernel_cfg[i]->set_arg(2, &_conv_weight_img[i], sizeof(_conv_weight_img[i]));
		_kernel_cfg[i]->set_arg(3, &w, sizeof(w));
		_kernel_cfg[i]->set_arg(4, &h, sizeof(h));	
		_kernel_cfg[i]->commit_args();	
	}

	_kernel_cfg[0]->set_global_work_size(w / 4, h);
	_kernel_cfg[1]->set_global_work_size(w / 4, h);
	_kernel_cfg[2]->set_global_work_size(w / 4, h);

	bool res = load_kernel_group_cfg(g_internal_group_cfg, NUM_INTERNALE_TUNED_DEVICE);

	if (res)
	{
		printf("load_kernel_group_cfg internal successfully !\n");
	}
	else
	{
		printf("load_kernel_group_cfg internal failed !\n");
	}

	res = load_kernel_group_cfg(_external_group_cfg.data(), _external_group_cfg.size());

	if (res)
	{
		printf("load_kernel_group_cfg external successfully !\n");
	}
	else
	{
		printf("load_kernel_group_cfg external failed !\n");
	}

	return true;
}

bool JndFilterCL::process(unsigned char *in, unsigned char *out, int w, int h, int stride, bool b_cl_finish)
{
//	cout << "process with clfinish: " << (b_cl_finish ? "yes" : "no") << endl;

	if (!_b_init)
	{
		_error_str = "Not Initialized!";
		cout << _error_str << endl;

		return false;
	}

	//printf("\n");
	cl_int status = CL_SUCCESS;

	if (_width != w || _height != h || _stride != stride)
	{
		prepare(w, h, stride, in, out);
	}

	CHECK_CL_ERROR(status);

	{
		//TimeProfiler t("  WriteImage2D");
		//for (int i = 0; i < w*h; i++)
		//{
		//	_temp_buf[i] = in[i] / 255.0f;
		//}
		//_cl_engine->WriteImage2D(_conv_inout_img[0], _temp_buf, w / 4, h);
		_cl_engine->WriteImage2D((cl_mem)_conv_inout_img[0], in, stride / 4, h, _b_mapping, false);
	}

	CHECK_CL_ERROR(status);


	if (_b_tuning)
	{
		for (int i = 0; i < 3; i++)
		{
			_kernel_cfg[i]->tuning();
		}

		dump_kernel_group_cfg();

	}
	else
	{   
		{
			//TimeProfiler t("  Kernel Run");
			_kernel_cfg[0]->run(_b_profiling);
			_kernel_cfg[1]->run(_b_profiling);
			_kernel_cfg[2]->run(_b_profiling, false, b_cl_finish);
			
		}

		if (_b_profiling)
		{
			for (int i = 0; i < 3; i++)
			{
				_kernel_cfg[i]->print_profile_data();
			}		
		}

		{
			//TimeProfiler t("  ReadImage2D");
			_cl_engine->ReadImage2D((cl_mem)_conv_inout_img[3], out, stride / 4, h, _b_mapping);
			//_cl_engine->ReadBuffer((cl_mem)_conv_inout_img[3], out, w*h, _b_mapping);
			//_cl_engine->ReadImage2D(_conv_inout_img[0], _temp_buf, w / 4, h);
			//for (int i = 0; i < w*h; i++)
			//{
			//	out[i] = _temp_buf[i] * 255.0f;
			//}
		}
	}


	return true;
}

