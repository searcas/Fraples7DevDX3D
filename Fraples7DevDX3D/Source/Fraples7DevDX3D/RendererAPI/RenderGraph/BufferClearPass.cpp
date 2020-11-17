#include "BufferClearPass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
namespace FraplesDev
{
	BufferClearPass::BufferClearPass(std::string name)
		:Pass(std::move(name))
	{
	}
	void BufferClearPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
	}
}