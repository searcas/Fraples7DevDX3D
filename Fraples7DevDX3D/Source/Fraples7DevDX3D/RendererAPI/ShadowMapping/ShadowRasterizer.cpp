#include "ShadowRasterizer.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
namespace FraplesDev
{
	ShadowRasterizer::ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp)
		:_mDepthBias(depthBias),_mSlopeBias(slopeBias),_mClamp(clamp)
	{
		ChangeDepthBiasParameters(gfx, _mDepthBias, _mSlopeBias, _mClamp);
	}
	void ShadowRasterizer::ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp)
	{
		_mDepthBias = depthBias;
		_mSlopeBias = slopeBias;
		_mClamp = clamp;

		INFOMAN(gfx);

		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC{ CD3D11_DEFAULT{} };
		rasterDesc.DepthBias = depthBias;
		rasterDesc.SlopeScaledDepthBias = slopeBias;
		rasterDesc.DepthBiasClamp = clamp;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &_mRasterizer));
	}
	float ShadowRasterizer::GetClamp() const
	{
		return _mClamp;
	}
	float ShadowRasterizer::GetSlopeBias() const
	{
		return _mSlopeBias;
	}
	int ShadowRasterizer::GetDepthBias() const
	{
		return _mDepthBias;
	}
	void ShadowRasterizer::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(_mRasterizer.Get()));
	}
}