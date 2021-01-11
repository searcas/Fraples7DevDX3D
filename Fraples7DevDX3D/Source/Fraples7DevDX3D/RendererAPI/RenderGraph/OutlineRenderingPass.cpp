#include "OutlineRenderingPass.h"
#include "GraphicAPI/Graphics.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/Stencil.h"
#include "RendererAPI/Rasterizer/Rasterizer.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
namespace FraplesDev
{
	OutlineRenderingPass::OutlineRenderingPass(Graphics& gfx, std::string name)
		: RenderQueuePass(std::move(name))
	{
		RegisterInput(BufferInput<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterInput(BufferInput<DepthStencil>::Make("depthStencil", _mDepthStencil));
		RegisterOutput(BufferOutput<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterOutput(BufferOutput<DepthStencil>::Make("depthStencil", _mDepthStencil));
		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Rasterizer::Resolve(gfx, false));
	}
}