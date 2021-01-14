#pragma once
#include "RenderQueuePass.h"
namespace FraplesDev
{
	class BlurOutlineRenderPass : public RenderQueuePass
	{
	public:
		BlurOutlineRenderPass(Graphics& gfx,std::string name,unsigned int fullWidth, unsigned int fullHeight);
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override;
	};
}
