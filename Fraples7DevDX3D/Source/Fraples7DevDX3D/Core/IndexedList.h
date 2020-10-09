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