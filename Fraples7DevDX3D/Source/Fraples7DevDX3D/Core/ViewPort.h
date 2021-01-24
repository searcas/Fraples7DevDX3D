#pragma once
#include "RendererAPI/GFXContext.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
namespace FraplesDev
{
	class ViewPort : public GfxContext
	{
	public:
		ViewPort(Graphics& gfx) :
			ViewPort(gfx, (float)gfx.GetWidth(), (float)gfx.GetHeight())
		{

		}
		ViewPort(Graphics& gfx, float width,float height)
			
		{
			vp.Width = width;
			vp.Height = height;
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
