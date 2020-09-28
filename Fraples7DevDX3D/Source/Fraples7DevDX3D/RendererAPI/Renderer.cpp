#include "Renderer.h"

namespace FraplesDev {
	
	
	void Renderer::Render(Graphics& gfx) const noexcept
	{
		for (auto& b : _mBinds)
		{
			b->Bind(gfx);
		}
		gfx.RenderIndexed(_mpIndexBuffer->GetCount());
	}

	void Renderer::AddBind(std::unique_ptr<GfxContext>bind) noexcept
	{
		assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		_mBinds.push_back(std::move(bind));
	}
	void Renderer::AddIndexBuffer(std::unique_ptr<IndexBuffer>ibuf)noexcept
	{
		assert("Attempting to add index buffer a second time." && _mpIndexBuffer == nullptr);
		_mpIndexBuffer = ibuf.get();
		_mBinds.push_back(std::move(ibuf));
	}
}