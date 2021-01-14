#include "ScaleOutlineRenderGraph.h"
#include "RendererAPI/RenderGraph/BufferClearPass.h"
#include "RendererAPI/RenderGraph/LambertianPass.h"
#include "RendererAPI/RenderGraph/OutlineMaskGenerationPass.h"
#include "RendererAPI/RenderGraph/OutlineRenderingPass.h"
#include "GraphicAPI/Graphics.h"
#include <memory>
namespace FraplesDev
{
	ScaleOutlineRenderGraph::ScaleOutlineRenderGraph(Graphics& gfx)
		: RenderGraph(gfx)
	{
		{
			{
				auto bcp = std::make_unique<BufferClearPass>("clear");
				bcp->SetSyncLinkage("renderTarget", "$.backbuffer");
				bcp->SetSyncLinkage("depthStencil", "$.masterDepth");
				AppendPass(std::move(bcp));
			}
			{
				auto lp = std::make_unique<LambertianPass>(gfx, "lambertian");
				lp->SetSyncLinkage("renderTarget", "clear.renderTarget");
				lp->SetSyncLinkage("depthStencil", "clear.depthStencil");
				AppendPass(std::move(lp));

			}
			{
				auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
				pass->SetSyncLinkage("depthStencil", "lambertian.depthStencil");
				AppendPass(std::move(pass));
			}
			{
				auto pass = std::make_unique<OutlineRenderingPass>(gfx, "outlineDraw");
				pass->SetSyncLinkage("renderTarget", "lambertian.renderTarget");
				pass->SetSyncLinkage("depthStencil", "outlineMask.depthStencil");
				AppendPass(std::move(pass));
			}

			SetSinkTarget("backbuffer", "outlineDraw.renderTarget");
			Finalize();
		}
	}
}