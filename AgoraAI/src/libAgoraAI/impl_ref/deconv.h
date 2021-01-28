#pragma once

#include "../core/deconv.h"

namespace ref
{
	class DeConv : public core::DeConv
	{
	public:	
		virtual bool run();	

		DeConv(string name, Tensor *in, Tensor *out, Tensor *weight, Tensor *bias, ConvParam para) 
			: core::DeConv(name, in, out, weight, bias, para)
		{
			
		}
	};
}