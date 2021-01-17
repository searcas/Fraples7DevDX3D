#include "BufferClearPass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"

namespace FraplesDev
{
	BufferClearPass::BufferClearPass(std::string name)
		:Pass(std::move(name))
	{
		RegisterSync(DirectBufferSync<BufferResource>::Make("buffer", _mBuffer));
		RegisterSource(DirectBufferSource<BufferResource>::Make("buffer", _mBuffer));
	}
	void BufferClearPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		_mBuffer->Clear(gfx);
	}
}