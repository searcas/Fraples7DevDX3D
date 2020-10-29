#include "VertexShader.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"

namespace FraplesDev
{
	VertexShader::VertexShader(Graphics& gfx, const std::string& path) :_mPath(path)
	{
		INFOMAN(gfx);

		FPL_GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{ path.begin(),path.end() }.c_str(), &_mpBytecodeBlob));
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