#pragma once

#include "../core/eltwise.h"

namespace ref 
{
	class Eltwise : public core::Eltwise
	{
	public:
		virtual bool run();

		Eltwise(string name, Tensor *in0, Tensor *in1, Tensor *out) 
			: core::Eltwise(name, in0, in1, out)
		{			
		}
	};
};