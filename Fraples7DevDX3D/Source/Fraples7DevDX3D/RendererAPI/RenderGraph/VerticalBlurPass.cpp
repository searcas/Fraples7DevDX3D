#include "VerticalBlurPass.h"
#include "PassInput.h"
#include "PassOutput.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/Stencil.h"
#include "RendererAPI/Blending/Blending.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
namespace FraplesDev
{
	VerticalBlurPass::VerticalBlurPass(std::string name, Graphics& gfx)
		: FullScreenPass(std::move(name), gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Blending::Resolve(gfx, true));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));

		RegisterInput(ImmutableInput<GfxContext>::Make("scratchIn", _mBlurScratchIn));
		RegisterInput(ImmutableInput<GfxContext>::Make("control", _mControl));
		RegisterInput(ImmutableInput<CachingPixelConstantBufferEx>::Make("direction", _mDirection));

		RegisterInput(BufferInput<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterInput(BufferInput<DepthStencil>::Make("depthStencil", _mDepthStencil));

		RegisterOutput(BufferOutput<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterOutput(BufferOutput<DepthStencil>::Make("depthStencil", _mDepthStencil));

	}

	void VerticalBlurPass::Execute(Graphics& gfx) const noexcept
	{
		auto buf = _mDirection->GetBuffer();
		buf["isHorizontal"] = false;
		_mDirection->SetBuffer(buf);

		_mControl->Bind(gfx);
		_mDirection->Bind(gfx);
		_mBlurScratchIn->Bind(gfx);
		FullScreenPass::Execute(gfx);
	}
}