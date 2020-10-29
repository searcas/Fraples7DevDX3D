#include "Topology.h"
#include "GFXContextCodex.h"
namespace FraplesDev
{


	FraplesDev::Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
		:_mType(type)
	{

	}

	void FraplesDev::Topology::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetPrimitiveTopology(_mType);
	}

	std::shared_ptr<GfxContext> FraplesDev::Topology::Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
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