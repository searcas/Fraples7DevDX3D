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
		void RenderWidgets(Graphics& gfx);
	private:
		void SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG);
		void SetKernelBox(int radius)noexcept(!IS_DEBUG);
	private:
		
		enum class KernelType
		{
			Gauss,
			Box,
		}_mKernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;

		std::shared_ptr<CachingPixelConstantBufferEx>_mBlurKernel;
		std::shared_ptr<CachingPixelConstantBufferEx>_mBlurDirection;
	};
}