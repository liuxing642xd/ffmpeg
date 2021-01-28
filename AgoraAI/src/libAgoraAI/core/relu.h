#pragma once

#include "layer.h"

namespace core
{
	class Relu : public Layer
	{
	public:
		Relu()
		{
			_op_type = OP_TYPE::RELU;
			_limit   = -1.0f;
		}

		Relu(string name, Tensor *in, Tensor *out, float limit = -1.0f)
		{
			_name = name;
			_op_type = OP_TYPE::RELU;

			_inputs.push_back(in);
			_outputs.push_back(out);
			_limit   = limit;	
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
	protected:
		float _limit;

	};
}