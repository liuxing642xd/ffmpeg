#pragma once

#include "layer.h"

enum PadType
{
	EDGE = 0
};

namespace core
{
	class Pad : public Layer
	{
	public:
		Pad()
		{
			_op_type  = OP_TYPE::PAD;	
			_pad_type = PadType::EDGE;

			_pad_l = 0; 
			_pad_r = 0; 
			_pad_t = 0; 
			_pad_b = 0; 
		}

		Pad(string name, Tensor *in, Tensor *out, int padl, int padr, int padt, int padb, PadType pad_type = PadType::EDGE)
		{

			_op_type  = OP_TYPE::PAD;	
			_name = name;

			_inputs.push_back(in);
			_outputs.push_back(out);


			_pad_l = padl;
			_pad_r = padr;
			_pad_t = padt;
			_pad_b = padb;

			_pad_type = PadType::EDGE;
		}

		virtual bool warmup()
		{
			int n = _inputs[0]->_n;
			int h = _inputs[0]->_h + _pad_t + _pad_b;
			int w = _inputs[0]->_w + _pad_l + _pad_r;
			int c = _inputs[0]->_c;
			DATA_FORMAT fmt = _inputs[0]->_fmt;

			_outputs[0]->reshape(n, h, w, c, fmt);

			return true;
		}

	protected:
		PadType _pad_type;

		int _pad_l; //left
		int _pad_r; //right
		int _pad_t; //top
		int _pad_b; //bottom
	};
}