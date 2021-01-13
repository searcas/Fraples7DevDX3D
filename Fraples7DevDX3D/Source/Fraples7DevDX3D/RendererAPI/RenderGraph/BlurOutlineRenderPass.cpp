#include "BlurOutlineRenderPass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/VertexShader.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/Stencil.h"
#include "RendererAPI/Blending/Blending.h"
namespace FraplesDev
{
	BlurOutlineRenderPass::BlurOutlineRenderPass(Graphics& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight)
		:RenderQueuePass(std::move(name))
	{
		_mRenderTarget = std::make_unique<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
		AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Blending::Resolve(gfx, false));
		RegisterOutput(ImmutableOutput<RenderTarget>::Make("scratchOut", _mRenderTarget));
	}
}