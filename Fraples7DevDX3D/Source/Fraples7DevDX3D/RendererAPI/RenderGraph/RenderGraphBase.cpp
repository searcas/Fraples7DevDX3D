#include "RenderGraphBase.h"
#include "RendererAPI/RenderGraph/BufferClearPass.h"
#include "RendererAPI/RenderGraph/LambertianPass.h"
#include "RendererAPI/RenderGraph/OutlineMaskGenerationPass.h"
#include "RendererAPI/RenderGraph/OutlineRenderingPass.h"
#include "GraphicAPI/Graphics.h"
#include <memory>
namespace FraplesDev
{
	BaseRenderGraph::BaseRenderGraph(Graphics& gfx)
		: RenderGraph(gfx)
	{
		{
			{
				auto bcp = std::make_unique<BufferClearPass>("clear");
				bcp->SetInputSource("renderTarget", "$.backbuffer");
				bcp->SetInputSource("depthStencil", "$.masterDepth");
				AppendPass(std::move(bcp));
			}
			{
				auto lp = std::make_unique<LambertianPass>(gfx, "lambertian");
				lp->SetInputSource("renderTarget", "clear.renderTarget");
				lp->SetInputSource("depthStencil", "clear.depthStencil");
				AppendPass(std::move(lp));

			}
			{
				auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
				pass->SetInputSource("depthStencil", "lambertian.depthStencil");
				AppendPass(std::move(pass));
			}
			{
				auto pass = std::make_unique<OutlineRenderingPass>(gfx, "outlineDraw");
				pass->SetInputSource("renderTarget", "lambertian.renderTarget");
				pass->SetInputSource("depthStencil", "outlineMask.depthStencil");
				AppendPass(std::move(pass));
			}

			SetSinkTarget("backbuffer", "outlineDraw.renderTarget");
			Finalize();
		}
	}
}