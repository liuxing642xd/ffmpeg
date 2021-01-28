#pragma once

#include "conv.h"
#include <cassert>

namespace core
{
	class DeConv : public Conv
	{
	public:

		DeConv() { _op_type = OP_TYPE::DECONV; }

		DeConv(string name, Tensor *in, Tensor *out, Tensor *weight, Tensor *bias, ConvParam para)
			: Conv(name, in, out, weight, bias, para)
		{
			_op_type = OP_TYPE::DECONV;
		}

		virtual bool warmup()
		{			
			assert(_para._stride_h == _para._stride_w);
			assert(_para._pad_t == _para._pad_b);
			assert(_para._pad_l == _para._pad_r);
			assert(_para._pad_t == _para._pad_l);
			assert(_para._kh == _para._kw);

			int s = _para._stride_h;
			int p = _para._pad_l;
			int k = _para._kw;

			assert(s > 1);
			assert(p > 0);

			int n = 1;
			int h = s*(_inputs[0]->_h - 1) + k - 2 * p;
			int w = s*(_inputs[0]->_w - 1) + k - 2 * p;
			int c = _weight->_n;

			DATA_FORMAT fmt = _inputs[0]->_fmt;

			_outputs[0]->reshape(n, h, w, c, fmt);

			return true;
		}
		 
	};
};