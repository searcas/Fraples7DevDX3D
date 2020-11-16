#pragma once
#include "GraphicAPI/Graphics.h"
#include "GraphicAPI/GraphicsResource.h"
namespace FraplesDev
{
	class DepthStencil : public GraphicsResource
	{
		friend class RenderTarget;
		friend class Graphics;
	public:
		DepthStencil(Graphics& gfx, UINT width, UINT height);
		void BindAsDepthStencil(Graphics& gfx)const noexcept;
		void Clear(Graphics& gfx)const noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>_mPdepthStencilView;
	};
}