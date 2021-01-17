#include "OutlineMaskGenerationPass.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/NullPixelShader.h"
#include "RendererAPI/Stencil.h"
#include "RendererAPI/Rasterizer/Rasterizer.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
namespace FraplesDev
{
	OutlineMaskGenerationPass::OutlineMaskGenerationPass(Graphics& gfx, std::string name)
		: RenderQueuePass(std::move(name))
	{
		RegisterSync(DirectBufferSync<DepthStencil>::Make("depthStencil", _mDepthStencil));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(NullPixelShader::Resolve(gfx));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Write));
		AddBind(Rasterizer::Resolve(gfx, false));
	}
}