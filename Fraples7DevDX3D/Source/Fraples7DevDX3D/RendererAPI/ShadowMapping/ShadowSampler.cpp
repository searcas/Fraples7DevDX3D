#include "ShadowSampler.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"

namespace FraplesDev
{
	ShadowSampler::ShadowSampler(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &_mSampler));
	}
	void ShadowSampler::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetSamplers(1, 1, _mSampler.GetAddressOf()));
	}
}