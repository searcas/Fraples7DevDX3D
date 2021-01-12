#pragma once
#include "RenderGraph.h"
#include <memory>
#include "RendererAPI/ConstantBuffersEx.h"

namespace FraplesDev
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(Graphics& gfx);
	private:
		void SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG);
	private:
		static constexpr int maxRadious = 15;
		static constexpr int radius = 4;
		static constexpr float sigma = 2.0f;

		std::shared_ptr<CachingPixelConstantBufferEx>_mBlurControl;
		std::shared_ptr<CachingPixelConstantBufferEx>_mBlurDirection;
	};
}