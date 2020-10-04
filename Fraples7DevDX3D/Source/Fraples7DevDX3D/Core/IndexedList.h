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

	private:
		std::vector<T> _mVertices;
		std::vector<unsigned short>_mIndices;
	};


}