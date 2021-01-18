#include "OutlineRenderingPass.h"
#include "GraphicAPI/Graphics.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/Rasterizer/Rasterizer.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
namespace FraplesDev
{
	OutlineRenderingPass::OutlineRenderingPass(Graphics& gfx, std::string name)
		: RenderQueuePass(std::move(name))
	{
		RegisterSync(DirectBufferSync<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSync(DirectBufferSync<DepthStencil>::Make("depthStencil", _mDepthStencil));
		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Rasterizer::Resolve(gfx, false));
	}
}