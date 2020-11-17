#include "RenderTarget.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
namespace FraplesDev
{
	RenderTarget::RenderTarget(Graphics& gfx, UINT width, UINT height)
		:_mWidth(width),_mHeight(height)
	{
		INFOMAN(gfx);

		//Create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = {  };
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>ptexture;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &ptexture));


		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(ptexture.Get(), &srvDesc, &_mPtextureView));

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(ptexture.Get(), &rtvDesc, &_mPtargetView));
	}
	void RenderTarget::Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept
	{
		GetContext(gfx)->ClearRenderTargetView(_mPtargetView.Get(), color.data());
	}
	void RenderTarget::Clear(Graphics& gfx) const noexcept
	{
		Clear(gfx, { 0.0f,0.0f,0.0f,0.0f });
	}
	void RenderTarget::BindAsTexture(Graphics& gfx, UINT slot)const
	{
		GetContext(gfx)->PSSetShaderResources(slot, 1u, _mPtextureView.GetAddressOf());
	}
	void RenderTarget::BindAsTarget(Graphics& gfx)const 
	{
		GetContext(gfx)->OMSetRenderTargets(1u, _mPtargetView.GetAddressOf(), nullptr);

		// cfg viewport
		D3D11_VIEWPORT vp = {};
		vp.Width = (float)_mWidth;
		vp.Height = (float)_mHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		GetContext(gfx)->RSSetViewports(1u, &vp);
	}
	void RenderTarget::BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept
	{
		GetContext(gfx)->OMSetRenderTargets(1, _mPtargetView.GetAddressOf(), depthStencil._mPdepthStencilView.Get());

		// cfg viewport
		D3D11_VIEWPORT vp = {};
		vp.Width = (float)_mWidth;
		vp.Height = (float)_mHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		GetContext(gfx)->RSSetViewports(1u, &vp);
	}
}