#include "VertexBuffer.h"

void FraplesDev::VertexBuffer::Bind(Graphics& gfx) noexcept
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, _mpVertexBuffer.GetAddressOf(), &_mStride, &offset);
}
