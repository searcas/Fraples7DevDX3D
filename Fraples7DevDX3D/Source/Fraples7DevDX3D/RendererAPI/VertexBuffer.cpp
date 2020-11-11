#include "VertexBuffer.h"
#include "GFXContextCodex.h"
namespace FraplesDev
{


	VertexBuffer::VertexBuffer(Graphics& gfx,const std::string& tag,const MP::VertexBuffer& vbuf)
		: _mStride((UINT)vbuf.GetLayout().Size()),_mTag(tag),_mLayout(vbuf.GetLayout())
	{
		INFOMAN(gfx);
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(vbuf.SizeOfBytes());
		bd.StructureByteStride = _mStride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.GetData();
		FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &_mpVertexBuffer));
	}

	VertexBuffer::VertexBuffer(Graphics& gfx, const MP::VertexBuffer& vbuf) : VertexBuffer(gfx, "?", vbuf)
	{
	}
	void FraplesDev::VertexBuffer::Bind(Graphics& gfx) noexcept
	{
		const UINT offset = 0u;
		GetContext(gfx)->IASetVertexBuffers(0u, 1u, _mpVertexBuffer.GetAddressOf(), &_mStride, &offset);
	}
	const MP::VertexLayout& VertexBuffer::GetLayout()const noexcept
	{
		return _mLayout;
	}

	std::shared_ptr<VertexBuffer> FraplesDev::VertexBuffer::Resolve(Graphics& gfx, const std::string& tag, const MP::VertexBuffer& vbuf)
	{
		assert(tag != "?");
		return Codex::Resolve<VertexBuffer>(gfx, tag, vbuf);
	}

	std::string FraplesDev::VertexBuffer::GetUID() const noexcept
	{
		return _mTag;
	}

	std::string FraplesDev::VertexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}
}