
#include "concat.h"
#include <algorithm>

namespace ref
{
	bool Concat::run()
	{
		int w = _outputs[0]->_w;
		int h = _outputs[0]->_h;

		float *pIn0  = _inputs[0]->f32();
		float *pIn1  = _inputs[1]->f32();
		int c0 = _inputs[0]->_c;
		int c1 = _inputs[1]->_c;

		float *pOut = _outputs[0]->f32();
				
		for (size_t i = 0; i < w*h; i++)
		{
			for (int j = 0; j < c0; j++)
			{
				*pOut++ = *pIn0++;
			}
			for (int j = 0; j < c1; j++)
			{
				*pOut++ = *pIn1++;
			}
		}
		
		return true;
	}
};

