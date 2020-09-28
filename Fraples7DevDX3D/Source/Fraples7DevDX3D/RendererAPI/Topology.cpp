#include "Topology.h"

FraplesDev::Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	:_mType(type)
{

}

void FraplesDev::Topology::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(_mType);
}
