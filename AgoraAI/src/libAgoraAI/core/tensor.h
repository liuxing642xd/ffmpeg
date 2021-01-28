#pragma once

#include <stddef.h>
#include "utils.h"

enum DATA_FORMAT
{
	NHWC = 0,
	NCHW
};

class Tensor
{
public:
	Tensor() 
	{
		_n = _h = _w = _c = 0;

		_data_f32 = 0;
		_b_shadow = false;
		_fmt = DATA_FORMAT::NHWC;
	}

	Tensor(int n, int h, int w, int c, DATA_FORMAT fmt, float *data = 0)
	{
		reshape(n, h, w, c, fmt, data);
	}

	bool reshape(int n, int h, int w, int c, DATA_FORMAT fmt, float *data = 0)
	{
		size_t old_size = size();

		_n = n;
		_h = h;
		_w = w;
		_c = c;
		_fmt = fmt;

		if (data != 0)
		{
			release();
			_data_f32 = data;
			_b_shadow = true;
		}
		else
		{
			size_t new_size = size();
			if (old_size != new_size || _b_shadow)
			{
				release();

				_data_f32 = new float[new_size];
			}

			_b_shadow = false;
		}

		return true;
		
	}

	virtual ~Tensor() 
	{ 
		release();
	}

	float *f32(DATA_FORMAT fmt = DATA_FORMAT::NHWC)
	{
		if (_data_f32)
		{
			size_t sz = size();
			if (fmt != _fmt)
			{
				float *new_data = new float[sz];

				if (fmt == DATA_FORMAT::NHWC && _fmt == DATA_FORMAT::NCHW)
				{
					nchw_2_nhwc(_data_f32, new_data, _n, _h, _w, _c);
				}

				_fmt = fmt;

				if (!_b_shadow) delete[]_data_f32;
				_data_f32 = new_data;
				_b_shadow = false;
			}
		}

		return _data_f32;
	}

	size_t size() { return _n*_h*_w*_c; }

	void release()
	{
		if (_data_f32 && !_b_shadow)
		{
			delete[]_data_f32;
			_data_f32 = 0;
		}
	}

	
public:
	int _n;
	int _h;
	int _w;
	int _c;
	DATA_FORMAT _fmt;

private:
	float *_data_f32;
	bool   _b_shadow;

};