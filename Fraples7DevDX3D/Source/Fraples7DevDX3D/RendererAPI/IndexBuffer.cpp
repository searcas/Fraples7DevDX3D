#include "IndexBuffer.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"

namespace FraplesDev
{
	IndexBuffer::IndexBuffer(Graphics& gfx,const std::vector<unsigned short>& indices)
		:_mCount((UINT)indices.size())
	{
		
		INFOMAN(gfx);
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = UINT(_mCount * sizeof(unsigned short));
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices.data();
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &_mpIndexBuffer));

	}
	void IndexBuffer::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetIndexBuffer(_mpIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	}
	UINT IndexBuffer::GetCount() const noexcept
	{
		return _mCount;
	}
}