#include "DepthStencil.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/RenderTarget.h"
#include "GraphicAPI/Graphics.h"
#include "Core/Surface.h"
#include "Core/Debugging/cnpy.h"
namespace FraplesDev
{
	DXGI_FORMAT MapUsageTypeless(DepthStencil::Usage usage)
	{
		switch (usage)
		{
		case FraplesDev::DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
			break;
		case FraplesDev::DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
			break;
		default:
			throw std::runtime_error("Bad usage for Typeless format map in DepthStencil");
			break;
		}
	}
	DXGI_FORMAT MapUsageTyped(DepthStencil::Usage usage)
	{
		switch (usage)
		{
		case FraplesDev::DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case FraplesDev::DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
			break;
		default:
			throw std::runtime_error{ "Bad usage for typed format map in DepthStencil" };
			break;
		}
	}
	DXGI_FORMAT MapUsageColored(DepthStencil::Usage usage)
	{
		switch (usage)
		{
		case FraplesDev::DepthStencil::Usage::DepthStencil:
			return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		case FraplesDev::DepthStencil::Usage::ShadowDepth:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			break;
		default:
			throw std::runtime_error{ "Bad usage for Colored format map in DepthStencil" };
			break;
		}
	}
	DepthStencil::DepthStencil(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face)
	{
		INFOMAN(gfx);
		D3D11_TEXTURE2D_DESC descTex = {};
		pTexture->GetDesc(&descTex);
		_mWidth = descTex.Width;
		_mHeight = descTex.Height;
		
		//create target view of depth stencil texture

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D32_FLOAT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.ArraySize = 1;
		descView.Texture2DArray.FirstArraySlice = face;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pTexture.Get(), &descView, &_mDepthStencilView));
	}
	DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput, DepthStencil::Usage usage)
		:_mWidth(width),_mHeight(height)
	{
		INFOMAN(gfx);

		//create depth stencil texture
		Microsoft::WRL::ComPtr<ID3D11Texture2D>pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = MapUsageTypeless(usage);
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

		// create target view of depth stencil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = MapUsageTyped(usage);
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;


		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), &descView, &_mDepthStencilView));
	}
	std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> DepthStencil::MakeStaging(Graphics& gfx) const
	{
		INFOMAN(gfx);
		D3D11_DEPTH_STENCIL_VIEW_DESC srcViewDesc{};
		_mDepthStencilView->GetDesc(&srcViewDesc);
		// creating a temp texture compatible with the source, but with CPU read access
		Microsoft::WRL::ComPtr<ID3D11Resource> pResSource;
		_mDepthStencilView->GetResource(&pResSource);
		Microsoft::WRL::ComPtr<ID3D11Texture2D>pTexSource;
		pResSource.As(&pTexSource);
		D3D11_TEXTURE2D_DESC srcTextureDesc{};
		pTexSource->GetDesc(&srcTextureDesc);
		D3D11_TEXTURE2D_DESC tmpTextureDesc = srcTextureDesc;
		tmpTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tmpTextureDesc.Usage = D3D11_USAGE_STAGING;
		tmpTextureDesc.BindFlags = 0;
		tmpTextureDesc.MiscFlags = 0;
		tmpTextureDesc.ArraySize = 1;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>pTexTemp;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&tmpTextureDesc, nullptr, &pTexTemp));

		// copy texture contents
		if (srcViewDesc.ViewDimension == D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY)
		{
			// source is actually inside a cubemap texture, use view info to find the correct slice and copy subresource
			FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), srcViewDesc.Texture2DArray.FirstArraySlice, nullptr));
		}
		else
		{
			FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->CopyResource(pTexTemp.Get(), pTexSource.Get()));
		}

		return { std::move(pTexTemp),srcTextureDesc };
	}
	void DepthStencil::BindAsBuffer(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetRenderTargets(0, nullptr, _mDepthStencilView.Get()));
	}
	void DepthStencil::BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noexcept(!IS_DEBUG)
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(gfx, static_cast<RenderTarget*>(renderTarget));
	}
	void DepthStencil::BindAsBuffer(Graphics& gfx, RenderTarget* renderTarget) noexcept(!IS_DEBUG)
	{
		renderTarget->BindAsBuffer(gfx, this);
	}
	void DepthStencil::BindAsDepthStencil(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		GetContext(gfx)->OMSetRenderTargets(0, nullptr, _mDepthStencilView.Get());
	}
	void DepthStencil::Clear(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		GetContext(gfx)->ClearDepthStencilView(_mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}
	Surface DepthStencil::ToSurface(Graphics& gfx, bool linearize ) const
	{
		INFOMAN(gfx);
		// copy from resource to staging
		auto [pTexTemp, srcTextureDesc] = MakeStaging(gfx);

		// create Surface and copy from temp texture to it
		const auto width = GetWidth();
		const auto height = GetHeight();
		Surface s{ width, height };
		D3D11_MAPPED_SUBRESOURCE msr{};
		FPL_GFX_THROW_INFO(GetContext(gfx)->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr));
		auto pSrcBytes = static_cast<const char*>(msr.pData);

		for (unsigned y = 0; y < height; y++)
		{
			struct Pixel
			{
				char data[4];
			};
			auto pSrcRow = reinterpret_cast<const Pixel*>(pSrcBytes + msr.RowPitch * size_t(y));
			for (unsigned x = 0; x < width; x++)
			{
				if (srcTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS)
				{
					const auto raw = 0xFFFFFF & *reinterpret_cast<const unsigned int*>(pSrcRow + x);
					if (linearize)
					{
						const auto normalized = (float)raw / (float)0xFFFFFF;
						const auto linearized = 0.01f / (1.01f - normalized);
						const auto channel = unsigned char(linearized * 255.0f);
						s.PutPixel(x, y, { channel,channel,channel });
					}
					else
					{
						const unsigned char channel = raw >> 16;
						s.PutPixel(x, y, { channel,channel,channel });
					}
				}
				else if (srcTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
				{
					const auto raw = *reinterpret_cast<const float*>(pSrcRow + x);
					if (linearize)
					{
						const auto linearized = 0.01f / (1.0f - raw);
						const auto channel = unsigned char(linearized * 255.0f);
						s.PutPixel(x, y, { channel,channel,channel });
					}
					else
					{
						const auto channel = unsigned char(raw * 255.0f);
						s.PutPixel(x, y, { channel,channel,channel });

					}
				}
				else
				{
					throw std::runtime_error{ "Bad format in Depth Stencil for conversion to Surface" };
				}
			}
		}
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->Unmap(pTexTemp.Get(), 0));
		return s;
	}
	void DepthStencil::Dumpy(Graphics& gfx, const std::string& path) const
	{
		INFOMAN(gfx);

		// copy from resource to staging
		auto [pTexTemp, srcTextureDesc] = MakeStaging(gfx);

		// create Surface and copy from temp texture to it

		const auto width = GetWidth();
		const auto height = GetHeight();

		std::vector<float>arr;
		arr.reserve(width * height);
		D3D11_MAPPED_SUBRESOURCE msr = {};
		FPL_GFX_THROW_INFO(GetContext(gfx)->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr));
		auto pSrcBytes = static_cast<const char*>(msr.pData);

		if (srcTextureDesc.Format != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
		{
			throw std::runtime_error{ "Bad format in Depth Stencil for dumpy" };
		}
		// flatten texture elements
		for (unsigned int y = 0; y < height; y++)
		{
			auto psrcRow = reinterpret_cast<const float*>(pSrcBytes + msr.RowPitch * size_t(y));
			for (unsigned int x = 0; x < width; x++)
			{
				arr.push_back(psrcRow[x]);
			}
		}
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->Unmap(pTexTemp.Get(), 0));
		cnpy::npy_save(path, arr.data(), { height,width });
	}
	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT slot, DepthStencil::Usage usage)
		:ShaderInputDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight(), slot, usage)
	{

	}
	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot, DepthStencil::Usage usage)
		: DepthStencil(gfx, width, height, true,usage), _mSlot(slot)
	{
		INFOMAN(gfx);
		Microsoft::WRL::ComPtr<ID3D11Resource>pRes;
		_mDepthStencilView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = MapUsageColored(usage); 
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pRes.Get(), &srvDesc, &_mShaderResourceView));
		
	}
	void ShaderInputDepthStencil::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(_mSlot, 1u, _mShaderResourceView.GetAddressOf()));
	}
	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture, UINT face)
		: DepthStencil(gfx, std::move(pTexture), face)
	{
	}
	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx)
		: OutputOnlyDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
	{
	}
	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height)
		: DepthStencil(gfx, width, height, false, Usage::DepthStencil)
	{
	}
	void OutputOnlyDepthStencil::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		assert("OutputonlyDepthStencil cannot be bound as shader input" && false);
	}
}