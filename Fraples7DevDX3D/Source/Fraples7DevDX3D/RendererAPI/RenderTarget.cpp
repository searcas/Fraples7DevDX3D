#include "RenderTarget.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "Core/Surface.h"

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
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // never do we not want to bind offscreen RTs as intpus
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>ptexture;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &ptexture));

		
		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(ptexture.Get(), &rtvDesc, &_mTargetView));
	}
	RenderTarget::RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture)
	{
		INFOMAN(gfx);
		// get information from texture about dimensions
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc(&textureDesc);
		_mWidth = textureDesc.Width;
		_mHeight = textureDesc.Height;

		// create the target view on the texture

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(pTexture, &rtvDesc, &_mTargetView));

	}
	void RenderTarget::BindAsBuffer(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		ID3D11DepthStencilView* const noll = nullptr;
		BindAsBuffer(gfx, noll);
	}
	void RenderTarget::BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noexcept(!IS_DEBUG)
	{
		assert(dynamic_cast<DepthStencil*>(depthStencil) != nullptr);
		BindAsBuffer(gfx, static_cast<DepthStencil*>(depthStencil));
	}
	void RenderTarget::BindAsBuffer(Graphics& gfx, DepthStencil* depthStencil) noexcept (!IS_DEBUG)
	{
		BindAsBuffer(gfx, depthStencil ? depthStencil->_mDepthStencilView.Get() : nullptr);
	}
	void RenderTarget::BindAsBuffer(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetRenderTargets(1, _mTargetView.GetAddressOf(), pDepthStencilView));

		//configure viewport

		D3D11_VIEWPORT vp;
		vp.Width = (float)_mWidth;
		vp.Height = (float)_mHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		GetContext(gfx)->RSSetViewports(1u, &vp);
	}
	void RenderTarget::Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->ClearRenderTargetView(_mTargetView.Get(), color.data()));
	}
	void RenderTarget::Clear(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		Clear(gfx, { 0.0f,0.0f,0.0f,0.0f });
	}
	UINT RenderTarget::GetWidth() const noexcept
	{
		return _mWidth;
	}

	UINT RenderTarget::GetHeight() const noexcept
	{
		return _mHeight;
	}

	ShaderInputRenderTarget::ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot)
		: RenderTarget(gfx,width,height),_mSlot(slot)
	{
		INFOMAN(gfx);
		Microsoft::WRL::ComPtr<ID3D11Resource>pRes;
		_mTargetView->GetResource(&pRes);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pRes.Get(), &srvDesc, &_mShaderResourceView));
		
	}

	Surface ShaderInputRenderTarget::ToSurface(Graphics& gfx) const
	{
		INFOMAN(gfx);
		
		// creating a temp texture compatible with the source, but with cpu read access
		Microsoft::WRL::ComPtr<ID3D11Resource>pResSource;
		_mShaderResourceView->GetResource(&pResSource);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As(&pTexSource);
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexSource->GetDesc(&textureDesc);
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>pTexTemp;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexTemp));

		// copy texture contents 
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->CopyResource(pTexTemp.Get(), pTexSource.Get()));

		//create Surface and copy from temp texture to it

		const auto width = GetWidth();
		const auto height = GetHeight();

		Surface s{ width,height };
		D3D11_MAPPED_SUBRESOURCE msr = {};
		FPL_GFX_THROW_INFO(GetContext(gfx)->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr));
		auto pSrcBytes = static_cast<const char*>(msr.pData);
		for (unsigned int y = 0; y < height; y++)
		{
			auto pSrcRow = reinterpret_cast<const Surface::Color*>(pSrcBytes + msr.RowPitch * size_t(y));
			for (unsigned int x = 0; x < width; x++)
			{
				s.PutPixel(x, y, *(pSrcRow + x));
			}
		}
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->Unmap(pTexTemp.Get(), 0));
		return s;
	}

	void ShaderInputRenderTarget::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(_mSlot, 1, _mShaderResourceView.GetAddressOf()));
	}

	void OutputOnlyRenderTarget::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		assert("Cannot bind OutputOnlyRenderTarget as shader input" && false);
	}
	OutputOnlyRenderTarget::OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture)
		:RenderTarget(gfx,pTexture)
	{

	}
}