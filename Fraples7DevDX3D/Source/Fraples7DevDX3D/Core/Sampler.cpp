#include "Sampler.h"
#include "Common\Exceptions\Macros\GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"
namespace FraplesDev
{

	Sampler::Sampler(Graphics& gfx)
	{
		INFOMAN(gfx);
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
	}
	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, pSampler.GetAddressOf());
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx)
	{
		return Codex::Resolve<Sampler>(gfx);
	}
	std::string Sampler::GenerateUID()
	{
		return typeid(Sampler).name();
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID();
	}
}