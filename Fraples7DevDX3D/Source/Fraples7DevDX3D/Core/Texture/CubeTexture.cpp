#include "CubeTexture.h"
#include "Core/Surface.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/RenderTarget.h"
#include <vector>
namespace FraplesDev
{
	CubeTexture::CubeTexture(Graphics& gfx, const std::string& path, UINT slot)
		:_mPath(path), _mSlot(slot)
	{
		INFOMAN(gfx);

		// load 6 surfaces for cube faces
		std::vector<Surface>surfaces;
		for (int i = 0; i < 6; i++)
		{
			surfaces.push_back(Surface::FromFile(path + "\\" + std::to_string(i) + ".png"));
		}

		// texture descriptor
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = surfaces[0].GetWidth();
		textureDesc.Height = surfaces[0].GetHeight();
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// subresource data
		constexpr short size = 0b0110;
		D3D11_SUBRESOURCE_DATA data[size];

		for (int i = 0; i < size; i++)
		{
			data[i].pSysMem = surfaces[i].GetBufferPtrConst();
			data[i].SysMemPitch = surfaces[i].GetBytePitch();
			data[i].SysMemSlicePitch = 0;
		}

		// create the texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, data, &pTexture));

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &_mTextureView));
	}
	void CubeTexture::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(_mSlot, 1u, _mTextureView.GetAddressOf()));
	}
	DepthCubeTexture::DepthCubeTexture(Graphics& gfx, UINT size, UINT slot)
		:_mSlot(slot)
	{
		INFOMAN(gfx);
		// Texture descriptor
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = size;
		textureDesc.Height = size;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		// create texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &_mTextureView));

		// make depth buffer resources for capturing shadow map

		for (UINT face = 0; face < 6; face++)
		{
			_mDepthBuffers.push_back(std::make_shared<OutputOnlyDepthStencil>(gfx, pTexture, face));
		}
	}
	void DepthCubeTexture::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(_mSlot, 1u, _mTextureView.GetAddressOf()));
	}
	std::shared_ptr<OutputOnlyDepthStencil> DepthCubeTexture::GetDepthBuffer(size_t index) const
	{
		return _mDepthBuffers[index];
	}
	CubeTargetTexture::CubeTargetTexture(Graphics& gfx, UINT width, UINT height, UINT slot, DXGI_FORMAT format)
		:_mSlot(slot)
	{
		INFOMAN(gfx);

		// texture descriptor
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// create the texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D>pTexture;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		// create the resource view on the texture

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &_mTextureView));

		//make render target resources for capturing shadow map

		for (UINT face = 0; face < 0b110; face++)
		{
			_mRenderTargets.push_back(std::make_shared<OutputOnlyRenderTarget>(gfx, pTexture.Get(), face));
		}


	}
	void CubeTargetTexture::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(_mSlot, 1u, _mTextureView.GetAddressOf()));
	}
	std::shared_ptr<OutputOnlyRenderTarget> CubeTargetTexture::GetRenderTarget(size_t index) const
	{
		return _mRenderTargets[_mSlot];
	}
}