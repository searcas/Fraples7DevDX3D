#pragma once
#include "RendererAPI/GFXContext.h"


namespace FraplesDev
{
	class ShadowSampler : public GfxContext
	{
	public:
		ShadowSampler(Graphics& gfx);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG)override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState>_mSampler;
	};

}
