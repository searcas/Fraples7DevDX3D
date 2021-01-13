#include "GraphicAPI/Graphics.h"
#include "Texture.h"
#include "Core/Surface.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include <algorithm>
namespace FraplesDev
{


	FraplesDev::Texture::Texture(Graphics& gfx, const std::string& path, UINT slot) :
		_mSlot(slot), _mPath(path)
	{
		INFOMAN(gfx);
		const auto s = Surface::FromFile(path);
		hasAlpha = s.AlphaLoaded();
		//create texture resource 
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture));

		//write image data into top mip level

		//write image data into top mip level
		GetContext(gfx)->UpdateSubresource(pTexture.Get(), 0u, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof(Surface::Color), 0u);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &_mpTextureView));

		//generate the mip chain using the gpu rendering pipeline
		GetContext(gfx)->GenerateMips(_mpTextureView.Get());
	}

	void FraplesDev::Texture::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(_mSlot, 1u, _mpTextureView.GetAddressOf()));
	}

	UINT Texture::CalculateNumberOfMipLevels(UINT width, UINT height) noexcept
	{
		const float xSteps = std::ceil(log2((float)width));
		const float ySteps = std::ceil(log2((float)height));

		return (UINT)std::max(xSteps, ySteps);
	}

	std::shared_ptr<Texture> FraplesDev::Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
	{
		return Codex::Resolve<Texture>(gfx, path, slot);
	}

	std::string FraplesDev::Texture::GenerateUID(const std::string& path, UINT slot)
	{

		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + std::to_string(slot);
	}
	
	std::string FraplesDev::Texture::GetUID() const noexcept
	{
		return GenerateUID(_mPath, _mSlot);
	}
}