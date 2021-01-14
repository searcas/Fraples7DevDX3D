#pragma once
#include "../RendererAPI/GFXContext.h"
#include <memory>
namespace FraplesDev
{

	class Sampler : public GfxContext
	{
	public:
		enum class Type
		{
			Anisotropic,
			Bilinear,
			Point
		};
		
		Sampler(Graphics& gfx, Type type, bool reflect);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
		static std::shared_ptr<Sampler>Resolve(Graphics& gfx, Type type = Type::Anisotropic, bool reflect = false);
		static std::string GenerateUID(Type type, bool reflect);
		std::string GetUID()const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		Type _mType;
		bool _mReflect;
	};

	
}