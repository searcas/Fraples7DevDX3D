#include "BlurOutlineRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineMaskGenerationPass.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "Core/Math/Math.h"
#include "BlurOutlineRenderPass.h"
namespace FraplesDev
{
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& gfx)
		: RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clear");
			pass->SetInputSource("renderTarget", "$.backbuffer");
			pass->SetInputSource("depthStencil", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetInputSource("renderTarget", "clear.renderTarget");
			pass->SetInputSource("depthStencil", "clear.depthStencil");
			AppendPass(std::move(pass));
		}
		// setup blur constant buffers
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
			pass->SetInputSource("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}
		// setup blur constant buffers
		{
			{
				MP::RawLayout l;
				l.Add<MP::Integer>("nTaps");
				l.Add<MP::Array>("coefficients");
				l["coefficients"].Set<MP::Float>(maxRadious * 2 + 1);
				MP::Buffer buf{ std::move(l) };
				_mBlurControl = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(ImmutableOutput<CachingPixelConstantBufferEx>::Make("blurControl", _mBlurControl));
			}
			{
				MP::RawLayout l;
				l.Add<MP::Bool>("isHorizontal");
				MP::Buffer buf{ std::move(l) };
				_mBlurDirection = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(ImmutableOutput<CachingPixelConstantBufferEx>::Make("blurDirection", _mBlurDirection));

			}
		}
		{
			auto pass = std::make_unique<BlurOutlineRenderPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetInputSource("scratchIn", "outlineDraw.scratchOut");
			pass->SetInputSource("control", "$.blurControl");
			pass->SetInputSource("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetInputSource("renderTarget", "lambertian.renderTarget");
			pass->SetInputSource("depthStencil", "outlineMask.depthStencil");
			pass->SetInputSource("scratchIn", "horizontal.scratchOut");
			pass->SetInputSource("control", "$.blurControl");
			pass->SetInputSource("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "vertical.renderTarget");
		Finalize();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG)
	{
		assert(radius <= maxRadious);
		auto k = _mBlurControl->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		float sum = 0.0f;
		for (int i = 0; i < nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x,sigma);
			sum += g;
			k["coefficients"][i] = g;
		}
		for (int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = (float)k["coefficients"][i] / sum;
		}
		_mBlurControl->SetBuffer(k);
	}
}