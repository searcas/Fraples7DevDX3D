#include "BufferClearPass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"

namespace FraplesDev
{
	BufferClearPass::BufferClearPass(std::string name)
		:Pass(std::move(name))
	{
		RegisterInput(BufferInput<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterInput(BufferInput<DepthStencil>::Make("depthStencil", _mDepthStencil));
		RegisterOutput(BufferOutput<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterOutput(BufferOutput<DepthStencil>::Make("depthStencil", _mDepthStencil));
	}
	void BufferClearPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		_mRenderTarget->Clear(gfx);
		_mDepthStencil->Clear(gfx);
	}
}