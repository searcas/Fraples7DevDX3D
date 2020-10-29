#include "InputLayout.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "GFXContextCodex.h"
namespace FraplesDev
{



	

	InputLayout::InputLayout(Graphics& gfx, MP::VertexLayout layout, ID3DBlob* pVertexShaderByteCode):_mLayout(std::move(layout))
	{
		INFOMAN(gfx);
		const auto d3dLayout = layout.GetD3DLayout();
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			 d3dLayout.data(), (UINT)d3dLayout.size(),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&_mInputLayout
		));
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(_mInputLayout.Get());
	}
	std::shared_ptr<GfxContext> InputLayout::Resolve(Graphics& gfx, const MP::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
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