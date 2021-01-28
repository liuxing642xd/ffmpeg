
#include "resize.h"
#include <string.h>


namespace ref
{
	bool Resize::run()
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

		switch (_resize_type)
		{
		case ResizeType::NEAREST_NEIGHBOR:
			for (int y = 0; y < out_height; y++)
			{
				float *pOut = out_data + y *out_width*c;

				int in_y = std::min(
					(align_corners) ? static_cast<int>(roundf(y * _height_scale)) : static_cast<int>(floorf(y * _height_scale)), in_height - 1);

				float *pIn = in_data + in_y *in_width*c;

				for (int x = 0; x < out_width; x++)
				{
					int in_x = std::min(
						(align_corners) ? static_cast<int>(roundf(x * _width_scale)) : static_cast<int>(floorf(x * _width_scale)), in_width - 1);

					for (int i = 0; i < c; i++)
					{
						pOut[x*c + i] = pIn[in_x*c + i];
					}
				}
			}		
			break;

		default:
			return false;

		}

		return true;
	}
};
