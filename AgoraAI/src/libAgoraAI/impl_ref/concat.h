#pragma once

#include "../core/concat.h"

namespace ref 
{
	class Concat : public core::Concat
	{
	public:
		virtual bool run();

		Concat(string name, Tensor *in0, Tensor *in1, Tensor *out) 
			: core::Concat(name, in0, in1, out)
		{			
		}
	};
};