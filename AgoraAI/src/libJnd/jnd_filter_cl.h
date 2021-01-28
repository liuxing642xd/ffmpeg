#pragma once

#include "jnd_filter.h"


struct JndGroupCfg
{
	string _device_name;
	string _device_version;
	int    _num_layers;

	struct
	{
		const char *_op_name;
		size_t  _group_size[3];
	}_layer_cfg[3];
	
};

class clEngine;
class clKernelCfg;

class JndFilterCL : public JndFilter
{
public:
	JndFilterCL();
	virtual ~JndFilterCL();

	/****************************************************************************
	    prefer_dev_type: defaultly when 0, device will be searched automatically
		( DEVICE_DEFAULT = 0, GPU_NVIDIA = 1, GPU_AMD = 2, GPU_MALI = 3, CPU_AMD = 4,
		  CPU_INTEL = 5, GPU_INTEL = 6, QUALCOMM = 7 )

        run_mode: normal = 0, profiling = 1, tuning = 2

		cl_bin_file: the opencl bin file's full path, if it does not exit, 
		             the bin will be build from source and saved.
	********************************************************************************/
	bool init(JND_NET_PARAS net_paras, string cl_bin_file="", int run_mode = 0, int prefer_dev_type=0, vector<JndGroupCfg> external_group_cfg = vector<JndGroupCfg>());

	virtual bool release();

	//
	virtual bool process(unsigned char *in, unsigned char *out, int w, int h, int stride, bool b_cl_finish = false);

	// not thread safe
	virtual bool update_net_paras(JND_NET_PARAS net_paras);
	
private:
	bool prepare(int w, int h, int stride, unsigned char *in, unsigned char *out);
	bool dump_kernel_group_cfg();
	bool load_kernel_group_cfg(JndGroupCfg *cfg, int num);

private:
	clEngine *_cl_engine;
	
	void *_conv_weight_img[3];	
	void *_conv_inout_img[4];

	clKernelCfg *_kernel_cfg[3];

	vector<JndGroupCfg> _external_group_cfg;

	bool _b_profiling;
	bool _b_tuning;

	bool _b_mapping;

	int _width;
	int _height;
	int _stride;

	float *_temp_buf;
	
};