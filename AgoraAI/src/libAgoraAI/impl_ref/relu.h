#pragma once

#include "../core/relu.h"

namespace ref 
{
	class Relu : public core::Relu
	{
	public:
		virtual bool run();

		Relu(string name, Tensor *in, Tensor *out, float limit = -1.0f) 
			: core::Relu(name, in, out, limit)
		{			
		}
	};
};