#include "conv.h"
#include "../core/tensor.h"

namespace ref
{
	bool Conv::run()
	{
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
						int pos_y = y*_para._stride_h + kh - _para._pad_t;

						if (pos_y >= 0 && pos_y < in_height)
						{
							for (int kw = 0; kw < _para._kw; kw++)
							{
								int pos_x = x*_para._stride_w + kw - _para._pad_l;

								if (pos_x >= 0 && pos_x < in_width)
								{
									float *pW  = weight_data + (n*_para._kh*_para._kw + kh*_para._kw + kw)*in_channel;
									float *pIn = in_data + (pos_y*in_width + pos_x)*in_channel;

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

					pOut[n] = sum;

				}
			}
		}
		return true;
	}
};
