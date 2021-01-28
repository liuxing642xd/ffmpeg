
#include "eltwise.h"
#include <algorithm>

namespace ref
{
	bool Eltwise::run()
	{
		float *pIn0  = _inputs[0]->f32();
		float *pIn1  = _inputs[1]->f32();

		float *pOut = _outputs[0]->f32();
		size_t sz   = _inputs[0]->size();
		
		for (size_t i = 0; i < sz; i++)
		{
			pOut[i] = pIn0[i] + pIn1[i];
		}
		
		return true;
	}
};

