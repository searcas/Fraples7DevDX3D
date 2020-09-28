#include "PixelShader.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"

namespace FraplesDev
{



	PixelShader::PixelShader(Graphics& gfx, const std::wstring& path)
	{
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3DBlob> pblob;
		FPL_GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pblob));
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pblob->GetBufferPointer(), pblob->GetBufferSize(), nullptr, &_mPixelShader));
	}

	void PixelShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShader(_mPixelShader.Get(), nullptr, 0u);
	}

}