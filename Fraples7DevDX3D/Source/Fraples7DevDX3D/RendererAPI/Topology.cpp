#include "Topology.h"
#include "GFXContextCodex.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
namespace FraplesDev
{


	FraplesDev::Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
		:_mType(type)
	{

	}

	void FraplesDev::Topology::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetPrimitiveTopology(_mType));
	}

	std::shared_ptr<Topology> FraplesDev::Topology::Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	{
		return Codex::Resolve<Topology>(gfx, type);
	}

	std::string FraplesDev::Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type) noexcept
	{
		using namespace std::string_literals;
		return typeid(Topology).name() + "#"s + std::to_string(type);
	}

	std::string FraplesDev::Topology::GetUID() const noexcept
	{
		return GenerateUID(_mType);
	}

}