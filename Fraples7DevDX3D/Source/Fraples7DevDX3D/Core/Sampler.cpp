#include "Sampler.h"
#include "Common\Exceptions\Macros\GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"
namespace FraplesDev
{

	Sampler::Sampler(Graphics& gfx, Type type, bool reflect, UINT slot)
		:_mType(type), _mReflect(reflect),_mSlot(slot)
	{
		INFOMAN(gfx);
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = [type]()
		{
			switch (type)
			{
			case FraplesDev::Sampler::Type::Anisotropic:
				return D3D11_FILTER_ANISOTROPIC;
				break;
			case FraplesDev::Sampler::Type::Bilinear:
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				break;
			case FraplesDev::Sampler::Type::Point:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
				break;
			default:
				break;
			}
		}();
		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR: D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
	}
	void Sampler::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetSamplers(_mSlot, 1, pSampler.GetAddressOf()));
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, Type type,bool reflect, UINT slot)
	{
		return Codex::Resolve<Sampler>(gfx, type, reflect, slot);
	}
	std::string Sampler::GenerateUID(Type type, bool reflect, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + std::to_string((int)type) + (reflect ? "R"s : "W"s) + "@"s + std::to_string(slot);
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(_mType,_mReflect,_mSlot);
	}
}