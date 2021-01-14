#include "BufferClearPass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"

namespace FraplesDev
{
	BufferClearPass::BufferClearPass(std::string name)
		:Pass(std::move(name))
	{
		RegisterSync(DirectBufferSnyc<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSync(DirectBufferSnyc<DepthStencil>::Make("depthStencil", _mDepthStencil));
		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
	}
	void BufferClearPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		_mRenderTarget->Clear(gfx);
		_mDepthStencil->Clear(gfx);
	}
}