#include "InputLayout.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"

namespace FraplesDev
{



	InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode)
	{
		INFOMAN(gfx);

		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			layout.data(), (UINT)layout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&_mInputLayout
		));
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(_mInputLayout.Get());
	}
}