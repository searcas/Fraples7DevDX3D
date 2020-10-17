#pragma once
#include "../RendererAPI/GFXContext.h"

namespace FraplesDev
{
	class Sampler : public GfxContext
	{
	public:
		Sampler(Graphics& gfx);
		void Bind(Graphics& gfx)noexcept override;


	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;

	};

	
}