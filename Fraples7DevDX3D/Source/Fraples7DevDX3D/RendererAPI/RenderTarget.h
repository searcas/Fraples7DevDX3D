#pragma once
#include "GraphicAPI/Graphics.h"
#include "GraphicAPI/GraphicsResource.h"


namespace FraplesDev
{
	class RenderTarget : public GraphicsResource
	{
	public:
		RenderTarget(Graphics& gfx, UINT width, UINT height);
		void BindAsTexture(Graphics& gfx, UINT slot);
		void BindAsTarget(Graphics& gfx);
		void BindAsTarget(Graphics& gfx, const class DepthStencil& depthStencil)const noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>_mPtextureView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>_mPtargetView;

	};
}
