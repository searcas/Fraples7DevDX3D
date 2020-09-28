#pragma once
#include "GFXContext.h"
namespace FraplesDev
{
	class PixelShader : public GfxContext
	{
	public:
		PixelShader(Graphics& gfx, const std::wstring& path);
		void Bind(Graphics& gfx) noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11PixelShader>_mPixelShader;
	};


}