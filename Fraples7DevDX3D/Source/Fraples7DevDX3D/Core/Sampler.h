#pragma once
#include "../RendererAPI/GFXContext.h"
#include <memory>
namespace FraplesDev
{
	class Sampler : public GfxContext
	{
	public:
		Sampler(Graphics& gfx);
		void Bind(Graphics& gfx)noexcept override;
		static std::shared_ptr<Sampler>Resolve(Graphics& gfx);
		static std::string GenerateUID();
		std::string GetUID()const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;

	};

	
}