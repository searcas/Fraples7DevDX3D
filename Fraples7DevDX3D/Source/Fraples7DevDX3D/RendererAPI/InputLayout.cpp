#include "InputLayout.h"
#include "../Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "VertexShader.h"
#include "GFXContextCodex.h"

namespace FraplesDev
{
	InputLayout::InputLayout(Graphics& gfx, MP::VertexLayout layout, const VertexShader&vs):_mLayout(std::move(layout))
	{
		INFOMAN(gfx);
		const auto d3dLayout = _mLayout.GetD3DLayout();
		const auto pByteCode = vs.GetByteCode();
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			 d3dLayout.data(), (UINT)d3dLayout.size(),
			pByteCode->GetBufferPointer(),
			pByteCode->GetBufferSize(),
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
	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,const MP::VertexLayout& layout, const VertexShader& vs)
	{
		return Codex::Resolve<InputLayout>(gfx, layout, vs);
	}
	std::string InputLayout::GenerateUID(const MP::VertexLayout& layout, const VertexShader& vs)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + _mLayout.GetCode() + "#"s + _mVertexShaderUID;
	}
}