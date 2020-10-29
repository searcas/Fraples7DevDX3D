#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Core/MetaProgramming/Vertex.h"

namespace FraplesDev
{
	class IndexedList
	{
	public:
		IndexedList() = default;

		IndexedList(MP::VertexBuffer vertices, std::vector<unsigned short>indices)
			: _mVertices(std::move(vertices)), _mIndices(std::move(indices))
		{
			assert(_mVertices.Size() > 2);
			assert(_mIndices.size() % 3 == 0);
		}

		void Transform(DirectX::FXMMATRIX matrix)
		{
			using Elements = MP::VertexLayout::ElementType;
			for (int i = 0; i < _mVertices.Size(); i++)
			{
				auto& pos = _mVertices[i].Attr<Elements::Position3D>();
				DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix));
			}
		}
		/*
		void SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
		{
			using namespace DirectX;
			assert(_mIndices.size() % 3 == 0 && _mIndices.size() > 0);
			for (size_t i = 0; i < _mIndices.size(); i += 3)
			{
				auto& v0 = _mVertices[_mIndices[i]];
				auto& v1 = _mVertices[_mIndices[i + 1]];
				auto& v2 = _mVertices[_mIndices[i + 2]];

				const auto p0 = DirectX::XMLoadFloat3(&v0.pos);
				const auto p1 = DirectX::XMLoadFloat3(&v1.pos);
				const auto p2 = DirectX::XMLoadFloat3(&v2.pos);

				const auto n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross((p1 - p0), (p2 - p0)));

				DirectX::XMStoreFloat3(&v0.normal, n);
				DirectX::XMStoreFloat3(&v1.normal, n);
				DirectX::XMStoreFloat3(&v2.normal, n);
			}
		}
		*/
	public:
		MP::VertexBuffer _mVertices;
		std::vector<unsigned short>_mIndices;
	};


	
}