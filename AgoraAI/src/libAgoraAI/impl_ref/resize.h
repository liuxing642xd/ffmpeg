#pragma once

#include "../core/resize.h"

namespace ref
{
	class Resize : public core::Resize
	{
	public:
		virtual bool run();

		Resize(string name, Tensor *in, Tensor *out, float height_scale, float width_scale, ResizeType resize_type = ResizeType::NEAREST_NEIGHBOR)
			: core::Resize(name, in, out, height_scale, width_scale, resize_type)
		{	

		}
	};
};
