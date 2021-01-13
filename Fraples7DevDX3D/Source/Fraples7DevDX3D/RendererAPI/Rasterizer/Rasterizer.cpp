#include "Rasterizer.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextCodex.h"
namespace FraplesDev
{
	Rasterizer::Rasterizer(Graphics& gfx, bool twoSided): _mTwoSide(twoSided)
	{
		INFOMAN(gfx);
		D3D11_RASTERIZER_DESC rasterDes = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDes.CullMode = _mTwoSide ? D3D11_CULL_NONE : D3D11_CULL_BACK;
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterDes, &_mRasterizerState));
	}
	void Rasterizer::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(_mRasterizerState.Get()));
	}
	std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& gfx, bool twoSided)
	{
		return Codex::Resolve<Rasterizer>(gfx, twoSided);
	}
	std::string Rasterizer::GenerateUID(bool twoSided)
	{
		using namespace std::string_literals;
		return typeid(Rasterizer).name() + "#"s + (twoSided ? "2s" : "1s");
		return std::string();
	}
	std::string Rasterizer::GetUID() const noexcept
	{
		return GenerateUID(_mTwoSide);
	}
}