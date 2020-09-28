#include "VertexShader.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"


namespace FraplesDev
{
	VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
	{
		INFOMAN(gfx);

		FPL_GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &_mpBytecodeBlob));

		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(_mpBytecodeBlob->GetBufferPointer(), _mpBytecodeBlob->GetBufferSize(), nullptr, &_mpVertexShader));

	}
	void VertexShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->VSSetShader(_mpVertexShader.Get(), nullptr, 0u);
	}
	ID3DBlob* VertexShader::GetBytecode() const noexcept
	{
		return _mpBytecodeBlob.Get();
	}
}