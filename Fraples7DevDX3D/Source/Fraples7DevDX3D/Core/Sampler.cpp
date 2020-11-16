#include "Sampler.h"
#include "Common\Exceptions\Macros\GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"
namespace FraplesDev
{

	Sampler::Sampler(Graphics& gfx, bool anisoEnable, bool reflect)
		:_mAnisoTropicEnable(anisoEnable), _mReflect(reflect)
	{
		INFOMAN(gfx);
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = _mAnisoTropicEnable ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR: D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
	}
	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, pSampler.GetAddressOf());
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, bool anisoEnable, bool reflect)
	{
		return Codex::Resolve<Sampler>(gfx, anisoEnable,  reflect);
	}
	std::string Sampler::GenerateUID(bool anisoEnable, bool reflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + (anisoEnable ? "A"s: "a"s) + (reflect ? "R"s:"W"s);
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(_mAnisoTropicEnable,_mReflect);
	}
}