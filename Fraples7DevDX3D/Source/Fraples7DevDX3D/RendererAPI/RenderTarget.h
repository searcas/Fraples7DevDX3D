#pragma once
#include "GraphicAPI/Graphics.h"
#include "GraphicAPI/GraphicsResource.h"
#include "RendererAPI/Stencil/DepthStencil.h"

namespace FraplesDev
{
	class RenderTarget : public GraphicsResource
	{
	public:
		RenderTarget(Graphics& gfx, UINT width, UINT height);
		void BindAsTexture(Graphics& gfx, UINT slot)const;
		void BindAsTarget(Graphics& gfx)const ;
		void BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil)const noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mPtextureView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>_mPtargetView;

	};
}
