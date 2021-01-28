#include "deconv.h"
#include "../core/tensor.h"

namespace ref
{
	bool DeConv::run()
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

		int pad_ = k - p - 1;
		int stride_ = 1;


		float *in_data     = _inputs[0]->f32();
		float *out_data    = _outputs[0]->f32();
		float *weight_data = _weight->f32();
		float *bias_data   = _bias ? _bias->f32() : 0;

		int out_width   = _outputs[0]->_w;
		int out_height  = _outputs[0]->_h;
		int in_width    = _inputs[0]->_w;
		int in_height   = _inputs[0]->_h;
		int out_channel = _outputs[0]->_c;
		int in_channel  = _inputs[0]->_c;

		for (int y = 0; y < out_height; y++)
		{
			for (int x = 0; x < out_width; x++)
			{	
				float *pOut = out_data + (y*out_width + x)*out_channel;
				for (int n = 0; n<out_channel; n++)
				{
					float sum = bias_data ? bias_data[n] : 0.0f;

					for (int kh = 0; kh < _para._kh; kh++)
					{
						int pos_y = y*stride_ + kh - pad_;

						if (pos_y >= 0 && pos_y < in_height*s)
						{
							if (pos_y % s == 0)
							{
								for (int kw = 0; kw < _para._kw; kw++)
								{
									int pos_x = x*stride_ +kw - pad_;

									if (pos_x >= 0 && pos_x < in_width*s)
									{
										if (pos_x % s == 0)
										{
											float *pW = weight_data + (n*_para._kh*_para._kw + kh*_para._kw + kw)*in_channel;
											float *pIn = in_data + ((pos_y/s)*in_width + (pos_x/s))*in_channel;

											for (int c = 0; c < in_channel; c++)
											{
												float w = pW[c];
												float pix = pIn[c];
												sum += pix * w;
											}
										}

									}
								}
							}
						}

					}

					pOut[n] = sum;

				}
			}
		}
		return true;
	}
};
