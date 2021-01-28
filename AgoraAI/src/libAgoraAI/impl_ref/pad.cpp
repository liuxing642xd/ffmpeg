
#include "pad.h"
#include <string.h>


namespace ref
{
	bool Pad::run()
	{
		int in_width = _inputs[0]->_w;
		int in_height = _inputs[0]->_h;
		int out_width = _outputs[0]->_w;
		int out_height = _outputs[0]->_h;
		int in_channel = _inputs[0]->_c;
		int out_channel = _outputs[0]->_c;
		float *in_data = _inputs[0]->f32();
		float *out_data = _outputs[0]->f32();

		int c = in_channel;

		switch (_pad_type)
		{
		case PadType::EDGE:
			for (int y = 0; y < in_height; y++)
			{
				float *pOut = out_data + (y + _pad_t)*out_width*c;
				float *pIn = in_data + y*in_width*c;
				memcpy(pOut + _pad_l*c, pIn, in_width*c * sizeof(float));

				for (int x = 0; x < _pad_l; x++)
				{
					memcpy(pOut + x*c, pIn, c * sizeof(float));
				}
				for (int x = 0; x < _pad_r; x++)
				{
					memcpy(pOut + (_pad_l + in_width + x)*c, pIn + (in_width - 1)*c, c * sizeof(float));
				}
			}

			for (int y = 0; y < _pad_t; y++)
			{
				memcpy(out_data + y*out_width*c, out_data + _pad_t*out_width*c, out_width*c * sizeof(float));
			}

			for (int y = 0; y < _pad_b; y++)
			{
				memcpy(out_data + (_pad_t + in_height + y)*out_width*c, out_data + (_pad_t + in_height - 1)*out_width*c, out_width*c * sizeof(float));
			}
			break;

		default:
			return false;

		}

		return true;
	}
};
