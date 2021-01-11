#pragma once
#include <vector>
#include <string>
#include "RenderQueuePass.h"
namespace FraplesDev
{
	class Graphics;
	class OutlineRenderingPass : public RenderQueuePass
	{
	public:
		OutlineRenderingPass(Graphics& gfx, std::string name);
	private:

	};
}