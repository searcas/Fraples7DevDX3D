#pragma once
#include <string>
#include "RenderQueuePass.h"
namespace FraplesDev
{
	class Graphics;
	class OutlineMaskGenerationPass : public RenderQueuePass
	{
	public:
		OutlineMaskGenerationPass(Graphics& gfx,std::string name);
	};
}