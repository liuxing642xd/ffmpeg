#pragma once

#include "../core/pad.h"

namespace ref
{
	class Pad : public core::Pad
	{
	public:
		virtual bool run();

		Pad(string name, Tensor *in, Tensor *out, int padl, int padr, int padt, int padb, PadType pad_type = PadType::EDGE) 
			: core::Pad(name, in, out, padl, padr, padt, padb, pad_type)
		{	
		}
	};
};
