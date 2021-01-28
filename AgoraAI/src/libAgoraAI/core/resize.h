#pragma once

#include "layer.h"
#include <algorithm>

enum ResizeType
{
	NEAREST_NEIGHBOR = 0,
	BILINEAR,
	BICUBIC
};

namespace core
{
	class Resize : public Layer
	{
	public:
		Resize()
		{
			_op_type     = OP_TYPE::RESIZE;	
			_resize_type = ResizeType::NEAREST_NEIGHBOR;

			_width_scale = 1.0f;
			_height_scale = 1.0f;

			align_corners = false;
		}

		Resize(string name, Tensor *in, Tensor *out, float height_scale, float width_scale, ResizeType resize_type = ResizeType::NEAREST_NEIGHBOR)
		{

			_op_type  = OP_TYPE::RESIZE;	
			_resize_type = resize_type;

			_name = name;

			_inputs.push_back(in);
			_outputs.push_back(out);

			_width_scale  = width_scale;
			_height_scale = height_scale;

			_resize_type = NEAREST_NEIGHBOR;

			align_corners = false;
		}

		virtual bool warmup()
		{
			int n = _inputs[0]->_n;
			int h = _inputs[0]->_h;
			int w = _inputs[0]->_w;
			int c = _inputs[0]->_c;

			DATA_FORMAT fmt = _inputs[0]->_fmt;

			h = roundf(h / _height_scale);
			w = roundf(w / _width_scale);

			_outputs[0]->reshape(n, h, w, c, fmt);

			return true;
		}

	protected:
		ResizeType _resize_type;

		float _width_scale; 
		float _height_scale; 

		bool align_corners;
		
	};
}