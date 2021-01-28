#pragma once

#include "../core/conv.h"

namespace ref
{
	class Conv : public core::Conv
	{
	public:	
		virtual bool run();	

		Conv(string name, Tensor *in, Tensor *out, Tensor *weight, Tensor *bias, ConvParam para) 
			: core::Conv(name, in, out, weight, bias, para)
		{
			
		}
	};
}