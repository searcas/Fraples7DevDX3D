#pragma once
#include "RendererAPI/GFXContext.h"


namespace FraplesDev
{
	class ShadowSampler : public GfxContext
	{
	public:
		ShadowSampler(Graphics& gfx);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG)override;
		void SetBilinear(bool bilin);
		void SetHwPcf(bool hwPcf);
		bool GetBilinear()const;
		bool GetHwPcf()const;
	private:
		static size_t ShadowSamplerIndex(bool bilin, bool hwPcf);
		static Microsoft::WRL::ComPtr<ID3D11SamplerState>MakeSampler(Graphics& gfx, bool bilin, bool hwPcf);
		UINT GetCurrentSlot();
	protected:
		size_t _mCurrentSampler = 0;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>_mSamplers[4];
	};

}
