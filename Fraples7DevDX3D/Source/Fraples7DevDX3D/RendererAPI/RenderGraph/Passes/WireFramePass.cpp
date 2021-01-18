#include "WireFramePass.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/RenderTarget.h"
namespace FraplesDev
{
	WireFramePass::WireFramePass(Graphics& gfx, std::string name)
		:RenderQueuePass(std::move(name))
	{
		RegisterSync(DirectBufferSync<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSync(DirectBufferSync<DepthStencil>::Make("depthStencil", _mDepthStencil));
		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthReversed));
	}
}
