#include "Texture.h"
#include "../Surface.h"
#include "../Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
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
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA subResData = {};
		subResData.pSysMem = s.GetBufferPtr();
		subResData.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, &subResData, &pTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &_mpTextureView));

	}

	void FraplesDev::Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(_mSlot, 1u, _mpTextureView.GetAddressOf());
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