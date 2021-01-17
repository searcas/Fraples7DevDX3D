#pragma once
#include "RenderQueuePass.h"

namespace FraplesDev
{
	class WireFramePass : public RenderQueuePass
	{
	public:
		WireFramePass(Graphics& gfx, std::string name);
	};
}