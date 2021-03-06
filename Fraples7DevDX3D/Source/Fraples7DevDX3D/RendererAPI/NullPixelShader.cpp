#include "NullPixelShader.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"
#include "Utility/Fraples7Utility.h"

namespace FraplesDev
{
	NullPixelShader::NullPixelShader(Graphics& gfx)
	{
	}
	void NullPixelShader::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShader(nullptr, nullptr, 0u));
	}
	std::shared_ptr<NullPixelShader> NullPixelShader::Resolve(Graphics& gfx)
	{
		return Codex::Resolve<NullPixelShader>(gfx);
	}
	std::string NullPixelShader::GenerateUID()
	{
		return typeid(NullPixelShader).name();
	}
	std::string NullPixelShader::GetUID() const noexcept
	{
		return GenerateUID();
	}
}