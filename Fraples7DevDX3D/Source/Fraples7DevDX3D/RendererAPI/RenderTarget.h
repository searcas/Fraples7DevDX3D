#pragma once
#include "GraphicAPI/Graphics.h"
#include "GraphicAPI/GraphicsResource.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include <array>
namespace FraplesDev
{
	class RenderTarget : public GraphicsResource
	{
	public:
		RenderTarget(Graphics& gfx, UINT width, UINT height);
		void Clear(Graphics& gfx, const std::array<float, 4>& color)const noexcept;
		void Clear(Graphics& gfx)const noexcept;
		void BindAsTexture(Graphics& gfx, UINT slot)const;
		void BindAsTarget(Graphics& gfx)const ;
		void BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil)const noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mPtextureView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>_mPtargetView;
		UINT _mWidth;
		UINT _mHeight;
	};
}
