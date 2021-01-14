#pragma once
#include "FullScreenPass.h"
#include "RendererAPI/ConstantBuffersEx.h"
namespace FraplesDev
{
	class HorizontalBlurPass : public FullScreenPass
	{
	public:
		HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight);
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override;
	private:
		std::shared_ptr<CachingPixelConstantBufferEx>_mDirection;
	};
}