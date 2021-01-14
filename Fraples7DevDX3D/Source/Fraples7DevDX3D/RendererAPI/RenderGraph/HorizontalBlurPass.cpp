#include "HorizontalBlurPass.h"
#include "RendererAPI/PixelShader.h"
#include "RendererAPI/Blending/Blending.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/ConstantBuffersEx.h"
#include "Core/Sampler.h"
namespace FraplesDev
{
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight)
		: FullScreenPass(std::move(name),gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "BlurOUtline_PS.cso"));
		AddBind(Blending::Resolve(gfx, false));
		AddBind(Sampler::Resolve(gfx,Sampler::Type::Point, true));
		AddBindSink<RenderTarget>("scratchIn");
		AddBindSink<CachingPixelConstantBufferEx>("kernel");
		RegisterSync(DirectContextSync<CachingPixelConstantBufferEx>::Make("direction", _mDirection));

		// the renderTarget is internally sourced and then exporeted as a Context
		_mRenderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		RegisterSource(DirectContextSource<RenderTarget>::Make("scratchOut", _mRenderTarget));
	}
	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)

	void HorizontalBlurPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		auto buf = _mDirection->GetBuffer();
		buf["isHorizontal"] = true;
		_mDirection->SetBuffer(buf);

		_mDirection->Bind(gfx);
		FullScreenPass::Execute(gfx);
	}
}