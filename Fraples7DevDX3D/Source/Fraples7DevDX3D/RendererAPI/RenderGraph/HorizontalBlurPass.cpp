#include "HorizontalBlurPass.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/Blending/Blending.h"
#include "RendererAPI/RenderTarget.h"

namespace FraplesDev
{
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight)
		: FullScreenPass(std::move(name),gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "BlurOUtline_PS.cso"));
		AddBind(Blending::Resolve(gfx, false));
		RegisterInput(ImmutableInput<GfxContext>::Make("control", _mControl));
		RegisterInput(ImmutableInput<CachingPixelConstantBufferEx>::Make("direction", _mDirection));
		RegisterInput(ImmutableInput<GfxContext>::Make("scratchIn", _mBlurScratchIn));

		// the renderTarget is internally sourced and then exporeted as a Context
		_mRenderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		RegisterOutput(ImmutableOutput<RenderTarget>::Make("scratchOut", _mRenderTarget));
	}
	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)

	void HorizontalBlurPass::Execute(Graphics& gfx) const noexcept
	{
		auto buf = _mDirection->GetBuffer();
		buf["isHorizontal"] = true;
		_mDirection->SetBuffer(buf);

		_mBlurScratchIn->Bind(gfx);
		_mControl->Bind(gfx);
		_mDirection->Bind(gfx);
		FullScreenPass::Execute(gfx);
	}
}