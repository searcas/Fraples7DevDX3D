#pragma once
#include "RendererAPI/GFXContext.h"

namespace FraplesDev
{
	class ShadowRasterizer : public GfxContext
	{
	public:
		ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp);
		void ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp);
		float GetClamp()const;
		float GetSlopeBias()const;
		int GetDepthBias()const;
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>_mRasterizer;
		int _mDepthBias;
		float _mSlopeBias;
		float _mClamp;
	};
}
