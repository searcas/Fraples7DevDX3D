#include "VerticalBlurPass.h"
#include "Sync.h"
#include "Source.h"
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

		RegisterSync(DirectContextSync<GfxContext>::Make("scratchIn", _mBlurScratchIn));
		RegisterSync(DirectContextSync<GfxContext>::Make("control", _mControl));
		RegisterSync(DirectContextSync<CachingPixelConstantBufferEx>::Make("direction", _mDirection));

		RegisterSync(DirectBufferSnyc<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSync(DirectBufferSnyc<DepthStencil>::Make("depthStencil", _mDepthStencil));

		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));

	}

	void VerticalBlurPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
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