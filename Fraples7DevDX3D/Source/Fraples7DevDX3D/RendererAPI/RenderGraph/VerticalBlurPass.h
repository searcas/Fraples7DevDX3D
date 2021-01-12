#pragma once
#include "FullScreenPass.h"
#include "RendererAPI/GFXContext.h"
#include "RendererAPI/ConstantBuffersEx.h"
namespace FraplesDev
{
	class VerticalBlurPass : public FullScreenPass
	{
	public:
		VerticalBlurPass(std::string name, Graphics& gfx);
		void Execute(Graphics& gfx)const noexcept override;
	private:
		std::shared_ptr<GfxContext>_mBlurScratchIn;
		std::shared_ptr<GfxContext>_mControl;
		std::shared_ptr<CachingPixelConstantBufferEx> _mDirection;
	};
}