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
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override;
	private:
		std::shared_ptr<CachingPixelConstantBufferEx> _mDirection;
	};
}