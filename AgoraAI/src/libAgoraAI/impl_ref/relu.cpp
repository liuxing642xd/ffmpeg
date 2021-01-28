
#include "relu.h"
#include <algorithm>

namespace ref
{
	bool Relu::run()
	{
		float *pIn  = _inputs[0]->f32();
		float *pOut = _outputs[0]->f32();
		size_t sz   = _inputs[0]->size();

		if (_limit < 0.0f)
		{
			for (size_t i = 0; i < sz; i++)
			{
				pOut[i] = std::max(0.0f, pIn[i]);
			}
		}
		else
		{
			for (size_t i = 0; i < sz; i++)
			{
				pOut[i] = std::max(0.0f, std::min(_limit, pIn[i]));
			}
		}

		return true;
	}
};

