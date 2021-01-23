#pragma once
#include "RendererAPI/GFXContext.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
namespace FraplesDev
{
	class ViewPort : public GfxContext
	{
	public:
		ViewPort(Graphics& gfx, float maxDepth = 1.0f)
		{
			vp.Width = (float)gfx.GetWidth();
			vp.Height = (float)gfx.GetHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY= 0.0f;
		}
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG)override
		{
			INFOMAN_NOHR(gfx);
			GetContext(gfx)->RSSetViewports(1u, &vp);
		}
	private:
		D3D11_VIEWPORT vp{};
	};


}
