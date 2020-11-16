#pragma once
#include "../RendererAPI/GFXContext.h"
#include <memory>
namespace FraplesDev
{
	class Sampler : public GfxContext
	{
	public:
		Sampler(Graphics& gfx, bool anisoEnable, bool reflect);
		void Bind(Graphics& gfx)noexcept override;
		static std::shared_ptr<Sampler>Resolve(Graphics& gfx, bool anisoEnable = true, bool reflect = false);
		static std::string GenerateUID(bool anisoEnable, bool reflect);
		std::string GetUID()const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		bool _mAnisoTropicEnable;
		bool _mReflect;
	};

	
}