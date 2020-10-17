#include "Texture.h"
#include "../Surface.h"
#include "../Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
FraplesDev::Texture::Texture(Graphics& gfx, const Surface& surfcae)
{
	INFOMAN(gfx);

	//create texture resource 
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = surfcae.GetWidth();
	textureDesc.Height = surfcae.GetHeight();
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
	subResData.pSysMem = surfcae.GetBufferPtr();
	subResData.SysMemPitch = surfcae.GetWidth() * sizeof(Surface::Color);
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
	GetContext(gfx)->PSSetShaderResources(0u, 1u, _mpTextureView.GetAddressOf());
}
