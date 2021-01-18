#include "VerticalBlurPass.h"
#include "RendererAPI/RenderGraph/Sync.h"

#include "RendererAPI/RenderGraph/Source.h"

#include "RendererAPI/PixelShader.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/Blending/Blending.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "Core/Sampler.h"
namespace FraplesDev
{
	VerticalBlurPass::VerticalBlurPass(std::string name, Graphics& gfx)
		: FullScreenPass(std::move(name), gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Blending::Resolve(gfx, true));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear, true));
		AddBindSink<RenderTarget>("scratchIn");
		AddBindSink<CachingPixelConstantBufferEx>("kernel");

		RegisterSync(DirectContextSync<CachingPixelConstantBufferEx>::Make("direction", _mDirection));

		RegisterSync(DirectBufferSync<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSync(DirectBufferSync<DepthStencil>::Make("depthStencil", _mDepthStencil));

		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));

	}

	void VerticalBlurPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		auto buf = _mDirection->GetBuffer();
		buf["isHorizontal"] = false;
		_mDirection->SetBuffer(buf);

		_mDirection->Bind(gfx);
		FullScreenPass::Execute(gfx);
	}
}