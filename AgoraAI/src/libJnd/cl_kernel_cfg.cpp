
#include "cl_kernel_cfg.h"
#include "cl_engine.h"
#include <iostream>

bool clKernelCfg::init(clEngine *cl_engine, string kernel_name, string op_name)
{
	cl_int status;


	_cl_engine = cl_engine;
	_kernel = _cl_engine->FindKernel(kernel_name);
	if (_kernel == 0)
	{
		printf("Can not find kernel %s\n", kernel_name.c_str());
		return false;
	}

	status = clGetKernelWorkGroupInfo(_kernel, _cl_engine->GetDevice(), CL_KERNEL_WORK_GROUP_SIZE, sizeof(_work_group_size), &_work_group_size, NULL);

	size_t *max_work_item_size = _cl_engine->GetMaxWorkItemSize();

	for (int i = 0; i < 3; i++)
	{
		_max_work_item_size[i] = max_work_item_size[i];
	}
	
	_kernel_name = kernel_name;
	_op_name     = op_name;

	cout << _kernel_name << " kernel work group size:" << _work_group_size << endl;

	return true;
}

bool clKernelCfg::set_arg(int idx, void *arg, size_t sz)
{
	int arg_num = _args.size();
	if (idx >= arg_num)
	{
		arg_num = idx + 1;
		_args.resize(arg_num);
	}
		
	_args[idx].resize(sz);
	memcpy(_args[idx].data(), arg, sz);

	return true;
}

void clKernelCfg::set_global_work_size(int width, int height, int depth)
{
	_global_work_size[0] = width;
	_global_work_size[1] = height;
	_global_work_size[2] = depth;

	int max_num_items = _global_work_size[0] * _global_work_size[1] * _global_work_size[2];
	if (max_num_items > _work_group_size) max_num_items = _work_group_size;

	if (max_num_items >= 256)
	{
		_local_work_size[0] = 16;
		_local_work_size[1] = 16;
		_local_work_size[2] = 1;
	}
	else if (max_num_items >= 64)
	{
		_local_work_size[0] = 8;
		_local_work_size[1] = 8;
		_local_work_size[2] = 1;
	}
	else if (max_num_items >= 16)
	{
		_local_work_size[0] = 4;
		_local_work_size[1] = 4;
		_local_work_size[2] = 1;
	}
	else
	{
		_local_work_size[0] = 1;
		_local_work_size[1] = 1;
		_local_work_size[2] = 1;
	}
	
}


void clKernelCfg::set_global_work_size(size_t size[3])
{
	set_global_work_size(size[0], size[1], size[2]);
}

void clKernelCfg::set_local_work_size(size_t size[3])
{
	_local_work_size[0] = size[0];
	_local_work_size[1] = size[1];
	_local_work_size[2] = size[2];
}

bool clKernelCfg::commit_args()
{
	cl_int status;

	for (int i = 0; i < _args.size(); i++)
	{
		status = clSetKernelArg(_kernel, i, _args[i].size(), _args[i].data()); 
	}


	CHECK_CL_ERROR(status);

	return true;
}

bool  clKernelCfg::run( bool b_profiling, bool b_flush, bool b_finish)
{
	cl_int status;

	size_t globalWorkSize[3];
	
	for (int i = 0; i < 3; i++)
	{
		globalWorkSize[i] = ROUND_UP(_local_work_size[i], _global_work_size[i]);
	}
	
	cl_command_queue queue = _cl_engine->GetQueue();

	int dim = globalWorkSize[2] == 1 ? 2 : 3;
	cl_event *pEvent = b_profiling ? &_profiling_event : 0;
	status = clEnqueueNDRangeKernel(queue, _kernel, dim, NULL, globalWorkSize, _local_work_size, 0, NULL, pEvent);

	if (b_flush)  clFlush(queue);
	if (b_finish) clFinish(queue);

	if (b_profiling)
	{		
		status = clWaitForEvents(1, &_profiling_event);

		cl_ulong queued, submit, start, end;
		
		clGetEventProfilingInfo(_profiling_event, CL_PROFILING_COMMAND_QUEUED, sizeof(queued), &queued, NULL);
		clGetEventProfilingInfo(_profiling_event, CL_PROFILING_COMMAND_SUBMIT, sizeof(submit), &submit, NULL);
		clGetEventProfilingInfo(_profiling_event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
		clGetEventProfilingInfo(_profiling_event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);

		_profile_data[0] = queued;
		_profile_data[1] = submit;
		_profile_data[2] = start;
		_profile_data[3] = end;

	}

	CHECK_CL_ERROR(status);


	return status == CL_SUCCESS;
}

void clKernelCfg::print_profile_data()
{	
	std::cout << _op_name << " SUB-QUE  " << (_profile_data[1] - _profile_data[0]) / 1e6 << " ms"
		<< "  STA-SUB  " << (_profile_data[2] - _profile_data[1]) / 1e6 << " ms"
		<< "  END-STA  " << (_profile_data[3] - _profile_data[2]) / 1e6 << " ms"
		<< " RAW"
		<< " QUE  " << _profile_data[0] << " SUB " << _profile_data[1]
		<< " STA  " << _profile_data[2] << " END " << _profile_data[3] << "\n";

		//if (i < KERNEL_NUM - 1) {
		//	std::cout << " start[" << i + 1 << "]-end[" << i << "] " << (profileData[i + 1][2] - profileData[i][3]) / 1e6 << "\n";
		//}
		//if (i == KERNEL_NUM - 1) {
		//	std::cout << " start[" << i << "]-end[" << 0 << "] " << (profileData[i][3] - profileData[0][2]) / 1e6 << "\n";
		//}
	
}

bool clKernelCfg::tuning()
{
	cl_int status = CL_SUCCESS;
	cl_command_queue queue = _cl_engine->GetQueue();
	size_t *max_group_size = _cl_engine->GetMaxWorkItemSize();
	
	int max_num_items = _global_work_size[0] * _global_work_size[1] * _global_work_size[2];
	if (max_num_items > _work_group_size) max_num_items = _work_group_size;

	cl_ulong t = CL_ULONG_MAX;
	for (int j = 1; j <= max_group_size[0]; j++)
	{
		//if (_global_work_size[0] % j) continue;
		for (int k = 1; k <= max_group_size[1]; k++)
		{
			//if (_global_work_size[1] % k) continue;
			if (j*k > max_num_items) continue;

			size_t localWorkSize[2] = { (size_t)j, (size_t)k };
			size_t globalWorkSize[2] = { ROUND_UP(j, _global_work_size[0]), ROUND_UP(k, _global_work_size[1]) };

			status = clEnqueueNDRangeKernel(queue, _kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, &_profiling_event);
			clFlush(queue);

			CHECK_CL_ERROR(status);
			if (status == CL_SUCCESS)
			{
				status = clWaitForEvents(1, &_profiling_event);
				cl_ulong start, end;
				clGetEventProfilingInfo(_profiling_event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
				clGetEventProfilingInfo(_profiling_event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);

				if (end - start < t)
				{
					t = end - start;

					_local_work_size[0] = j;
					_local_work_size[1] = k;
				}
			}
		}

	}
	return true;
}