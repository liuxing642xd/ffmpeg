#pragma once

#if __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include <string>
#include <vector>

using namespace std;

class clEngine;


class clKernelCfg
{
public:
	
	clKernelCfg()
	{
		for (int i = 0; i < 3; i++)
		{
			_local_work_size[i]  = 1;
			_global_work_size[i] = 1;
		}
	}


	bool init(clEngine *cl_engine, string kernel_name, string op_name = "Unknown");
	bool set_arg(int idx, void *arg, size_t sz);
	void set_global_work_size(int width, int height, int depth=1);
	void set_global_work_size(size_t size[3]);
	void set_local_work_size(size_t size[3]);
	bool commit_args();
	bool run(bool b_profiling = false, bool b_flush = false, bool b_finish = false);
	bool tuning();

	void print_profile_data();

	cl_kernel get_kernel() { return _kernel;  }
	const size_t *get_local_work_size() const { return _local_work_size;  }
	string get_op_name() { return _op_name;  }
private:

	clEngine *_cl_engine;

	string _kernel_name;
	string _op_name;
	
	cl_kernel _kernel;
	cl_event  _profiling_event;
	cl_ulong  _profile_data[4];

	size_t _work_group_size;

	size_t _max_work_item_size[3];
	size_t _local_work_size[3];
	size_t _global_work_size[3];

	vector<vector<unsigned char>> _args;
	
};
