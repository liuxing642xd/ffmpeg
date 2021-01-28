#pragma once

#include "layer.h"
#include <cassert>

namespace core
{
	class Concat : public Layer
	{
	public:
		Concat()
		{
			_op_type = OP_TYPE::CONCAT;	
		}

		Concat(string name, Tensor *in0, Tensor *in1, Tensor *out)
		{
			_name = name;
			_op_type = OP_TYPE::CONCAT;

			_inputs.push_back(in0);
			_inputs.push_back(in1);
			_outputs.push_back(out);			
		}

		virtual bool warmup()
		{
			int n0 = _inputs[0]->_n;
			int h0 = _inputs[0]->_h;
			int w0 = _inputs[0]->_w;
			int c0 = _inputs[0]->_c;

			int n1 = _inputs[1]->_n;
			int h1 = _inputs[1]->_h;
			int w1 = _inputs[1]->_w;
			int c1 = _inputs[1]->_c;

			assert(n0 == n1 && h0 == h1 && w0 == w1);

			DATA_FORMAT fmt = _inputs[0]->_fmt;

			_outputs[0]->reshape(n0, h0, w0, c0+c1, fmt);

			return true;
		}	
	};
}