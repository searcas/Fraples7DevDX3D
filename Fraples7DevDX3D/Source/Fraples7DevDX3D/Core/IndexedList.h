#pragma once
#include <vector>
#include <DirectXMath.h>


namespace FraplesDev
{
	template<typename T>
	class IndexedList
	{
	public:
		IndexedList() = default;
		IndexedList(std::vector<T>vertices, std::vector<unsigned short>indices);
		void Transform(DirectX::FXMMATRIX matrix);
		void SetNormalsIndependentFlat()noexcept(!IS_DEBUG)
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
	public:
		std::vector<T> _mVertices;
		std::vector<unsigned short>_mIndices;
	};

	template<typename T>
	IndexedList<T>::IndexedList(std::vector<T>vertices, std::vector<unsigned short>indices)
		: _mVertices(std::move(vertices)), _mIndices(std::move(indices))
	{
		assert(_mVertices.size() > 2);
		assert(_mIndices.size() % 3 == 0);
	}
	template<typename T>
	void IndexedList<T>::Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : _mVertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(&v.pos, DirectX::XMVector3Transform(pos, matrix));
		}
	}
	
}