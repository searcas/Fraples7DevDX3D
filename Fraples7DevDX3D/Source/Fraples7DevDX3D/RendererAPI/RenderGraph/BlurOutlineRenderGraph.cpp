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
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSyncLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSyncLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSyncLinkage("renderTarget", "clearRT.buffer");
			pass->SetSyncLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		// setup blur constant buffers
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
			pass->SetSyncLinkage("depthStencil", "lambertian.depthStencil");
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
				_mBlurKernel = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(DirectContextSource<CachingPixelConstantBufferEx>::Make("blurKernel", _mBlurKernel));
			}
			{
				MP::RawLayout l;
				l.Add<MP::Bool>("isHorizontal");
				MP::Buffer buf{ std::move(l) };
				_mBlurDirection = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(DirectContextSource<CachingPixelConstantBufferEx>::Make("blurDirection", _mBlurDirection));

			}
		}
		{
			auto pass = std::make_unique<BlurOutlineRenderPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetSyncLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSyncLinkage("kernel", "$.blurKernel");
			pass->SetSyncLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetSyncLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSyncLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSyncLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSyncLinkage("kernel", "$.blurKernel");
			pass->SetSyncLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "vertical.renderTarget");
		Finalize();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noexcept(!IS_DEBUG)
	{
		assert(radius <= maxRadious);
		auto k = _mBlurKernel->GetBuffer();
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
		_mBlurKernel->SetBuffer(k);
	}
}