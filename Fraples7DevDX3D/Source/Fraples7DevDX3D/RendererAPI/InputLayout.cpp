#include "InputLayout.h"
#include "../Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "GFXContextCodex.h"
namespace FraplesDev
{



	

	InputLayout::InputLayout(Graphics& gfx, MP::VertexLayout layout, ID3DBlob* pVertexShaderByteCode):_mLayout(std::move(layout))
	{
		INFOMAN(gfx);
		const auto d3dLayout = _mLayout.GetD3DLayout();
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			 d3dLayout.data(), (UINT)d3dLayout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&_mInputLayout
		));
	}

	void InputLayout::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetInputLayout(_mInputLayout.Get()));
	}
	const MP::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return _mLayout;
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,const MP::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
	{
		return Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderByteCode);
	}
	std::string InputLayout::GenerateUID(const MP::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		return GenerateUID(_mLayout);
	}
}