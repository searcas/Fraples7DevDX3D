#include "VertexShader.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"
#include "Utility/Fraples7Utility.h"
#include "GraphicAPI/Graphics.h"
namespace FraplesDev
{
	VertexShader::VertexShader(Graphics& gfx, const std::string& path) :_mPath(path)
	{
		INFOMAN(gfx);

		FPL_GFX_THROW_INFO(D3DReadFileToBlob(Utility::ToWide("Shaders/"+ path).c_str(), &_mpBytecodeBlob));
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(_mpBytecodeBlob->GetBufferPointer(), _mpBytecodeBlob->GetBufferSize(), nullptr, &_mpVertexShader));

	}
	void VertexShader::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->VSSetShader(_mpVertexShader.Get(), nullptr, 0u));
	}
	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return _mpBytecodeBlob.Get();
	}
	//path = "PhongDiffuseMaskSpecular_VS.cso"
	std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return Codex::Resolve<VertexShader>(gfx, path);
	}
	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}
	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID(_mPath);
	}
}