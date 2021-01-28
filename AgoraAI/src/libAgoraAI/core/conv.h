#pragma once

#include "layer.h"

class Tensor;

struct ConvParam
{
	int _kw;     //
	int _kh;     //
	int _pad_l;
	int _pad_r;
	int _pad_t;
	int _pad_b;
	int _stride_h;
	int _stride_w;	
};

namespace core
{
	class Conv : public Layer
	{
	public:

		Conv() { _op_type = OP_TYPE::CONV; }


		Conv(string name, Tensor *in, Tensor *out, Tensor *weight, Tensor *bias, ConvParam para)
		{
			_op_type = OP_TYPE::CONV;
			_name = name;

			_inputs.push_back(in);
			_outputs.push_back(out);
			_weight = weight;
			_bias   = bias;

			_para   = para;			
		}

		virtual bool warmup()
		{
			int n = 1;
			int h = (_inputs[0]->_h + _para._pad_t + _para._pad_b - _para._kh)/_para._stride_h + 1 ;
			int w = (_inputs[0]->_w + _para._pad_l + _para._pad_r - _para._kw)/_para._stride_w + 1;
			int c = _weight->_n;

			DATA_FORMAT fmt = _inputs[0]->_fmt;

			_outputs[0]->reshape(n, h, w, c, fmt);

			return true;
		}
		 

	protected:
		ConvParam _para;
		Tensor   *_weight;
		Tensor   *_bias;
	};
};