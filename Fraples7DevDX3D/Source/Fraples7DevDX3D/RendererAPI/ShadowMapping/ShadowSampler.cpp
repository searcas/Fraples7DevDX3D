#include "ShadowSampler.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"

namespace FraplesDev
{
	
	ShadowSampler::ShadowSampler(Graphics& gfx)
		:_mCurrentSampler(0)
	{
		for (size_t i = 0; i < 4; i++)
		{
			_mCurrentSampler = i;
			_mSamplers[i] = MakeSampler(gfx, GetBilinear(), GetHwPcf());
		}
		SetBilinear(true);
		SetHwPcf(true);
	}
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowSampler::MakeSampler(Graphics& gfx, bool bilin, bool hwPcf)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

		if (hwPcf)
		{
			samplerDesc.Filter = bilin ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		}
		else
		{
			samplerDesc.Filter = bilin ? D3D11_FILTER_MIN_MAG_MIP_LINEAR: D3D11_FILTER_MIN_MAG_MIP_POINT;

		}
		Microsoft::WRL::ComPtr<ID3D11SamplerState>pSampler;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
		return std::move(pSampler);
	}
	UINT ShadowSampler::GetCurrentSlot()
	{
		return GetHwPcf() ? 1 : 2;
	}
	void ShadowSampler::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetSamplers(GetCurrentSlot(), 1, _mSamplers[_mCurrentSampler].GetAddressOf()));
	}
	void ShadowSampler::SetBilinear(bool bilin)
	{
		_mCurrentSampler = (_mCurrentSampler & ~0b01) | (bilin ? 0b01 : 0);
	}
	void ShadowSampler::SetHwPcf(bool hwPcf)
	{
		_mCurrentSampler = (_mCurrentSampler & ~0b10) | (hwPcf ? 0b10 : 0);
	}
	bool ShadowSampler::GetBilinear() const
	{
		return _mCurrentSampler & 0b01;
	}
	bool ShadowSampler::GetHwPcf() const
	{
		return _mCurrentSampler & 0b10;;
	}
	size_t ShadowSampler::ShadowSamplerIndex(bool bilin, bool hwPcf)
	{
		return (bilin ? 0b01 : 0) + (hwPcf ? 0b10 : 0);
	}

}