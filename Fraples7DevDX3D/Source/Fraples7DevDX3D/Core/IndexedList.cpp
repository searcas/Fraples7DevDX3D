#include "IndexedList.h"

namespace FraplesDev
{
	template<typename T>
	IndexedList<T>::IndexedList(std::vector<T>vertices, std::vector<unsigned short>indices) 
		: _mVertices(std::move(vertices)),_mIndices(std::move(indices))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
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