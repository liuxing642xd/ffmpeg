#pragma once

#include "layer.h"

namespace core
{
	class Eltwise : public Layer
	{
	public:
		Eltwise()
		{
			_op_type = OP_TYPE::ELTWISE;	
		}

		Eltwise(string name, Tensor *in0, Tensor *in1, Tensor *out)
		{
			_name = name;
			_op_type = OP_TYPE::ELTWISE;

			_inputs.push_back(in0);
			_inputs.push_back(in1);
			_outputs.push_back(out);			
		}

		virtual bool warmup()
		{
			int n = _inputs[0]->_n;
			int h = _inputs[0]->_h;
			int w = _inputs[0]->_w;
			int c = _inputs[0]->_c;
			DATA_FORMAT fmt = _inputs[0]->_fmt;

			_outputs[0]->reshape(n, h, w, c, fmt);

			return true;
		}	
	};
}