#pragma once

#include "GFXContext.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "../Core/MetaProgramming/Vertex.h"

namespace FraplesDev
{
	class VertexBuffer : public GfxContext
	{
	public:
		template<class V>
		VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
			:_mStride(sizeof(V))
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC  bd = {};
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = UINT(sizeof(V)* vertices.size());
			bd.StructureByteStride = sizeof(V);
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = vertices.data();
			FPL_GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &_mpVertexBuffer));


		}
		VertexBuffer(Graphics& gfx, const MP::VertexBuffer& vbuf) 
			: _mStride((UINT)vbuf.GetLayout().Size())
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
		void Bind(Graphics& gfx)noexcept override;
	private:
		UINT _mStride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _mpVertexBuffer;

	};

}