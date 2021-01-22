#pragma once
#include "RenderGraph.h"
#include <memory>
#include "RendererAPI/ConstantBuffersEx.h"

namespace FraplesDev
{
	class Camera;
	class ShadowSampler;
	class ShadowRasterizer;
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(Graphics& gfx);
		void RenderWindows(Graphics& gfx);
		void BindMainCamera(Camera& cam);
		void BindShadowCamera(Camera& cam);
		void DumpShadowMap(Graphics& gfx, const std::string& path);
	private:
		void SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG);
		void SetKernelBox(int radius)noexcept(!IS_DEBUG);
		void RenderKernelWindow(Graphics& gfx);
		void RenderShadowWindow(Graphics& gfx);
	private:
		
		enum class KernelType
		{
			Gauss,
			Box,
		}_mKernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<CachingPixelConstantBufferEx>_mShadowControl;
		std::shared_ptr<CachingPixelConstantBufferEx>_mBlurKernel;
		std::shared_ptr<CachingPixelConstantBufferEx>_mBlurDirection;
		std::shared_ptr<ShadowSampler>_mShadowSampler;
		std::shared_ptr<ShadowRasterizer>_mShadowRasterizer;
	};
}