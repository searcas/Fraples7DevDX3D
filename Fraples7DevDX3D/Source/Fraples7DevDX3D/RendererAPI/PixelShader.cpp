#include "PixelShader.h"
#include "Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "GFXContextCodex.h"
namespace FraplesDev
{





	PixelShader::PixelShader(Graphics& gfx, const std::string& path) 
		:_mPath(path)
	{
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3DBlob> pblob;
		FPL_GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{ path.begin(),path.end() }.c_str(), &pblob));
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pblob->GetBufferPointer(), pblob->GetBufferSize(), nullptr, &_mPixelShader));
	}
	void PixelShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShader(_mPixelShader.Get(), nullptr, 0u);
	}

	std::shared_ptr<PixelShader> PixelShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return Codex::Resolve<PixelShader>(gfx,path	);
	}

	std::string PixelShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;

		return typeid(PixelShader).name() + "#"s + path;
	}

	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID(_mPath);
	}

}