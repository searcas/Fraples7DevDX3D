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

	void Renderer::AddBind(std::shared_ptr<GfxContext>bind) noexcept(!IS_DEBUG)
	{
		if (typeid(*bind) ==typeid(IndexBuffer))
		{
			assert("Binding multiple index buffers not allowed" && _mpIndexBuffer == nullptr);
			_mpIndexBuffer = &static_cast<IndexBuffer&>(*bind);
		}
		_mBinds.push_back(std::move(bind));
	}
}