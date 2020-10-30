#pragma once
#include <DirectXMath.h>
#include "../Core/IndexedList.h"
#include <initializer_list>
namespace FraplesDev
{


	class Cube
	{
	public:

		static IndexedList MakeIndependent(MP::VertexLayout layout)
		{
			MP::VertexBuffer vertices(std::move(layout),24u);
			constexpr float side = 1.0f / 2.0f;

			vertices[0].Attr<MP::ElementType::Position3D>() = { -side,-side,-side };// 0 near side
			vertices[1].Attr<MP::ElementType::Position3D>() = { side,-side,-side };// 1
			vertices[2].Attr<MP::ElementType::Position3D>() = { -side,side,-side };// 2
			vertices[3].Attr<MP::ElementType::Position3D>() = { side,side,-side };// 3
			vertices[4].Attr<MP::ElementType::Position3D>() = { -side,-side,side };// 4 far side
			vertices[5].Attr<MP::ElementType::Position3D>() = { side,-side,side };// 5
			vertices[6].Attr<MP::ElementType::Position3D>() = { -side,side,side };// 6
			vertices[7].Attr<MP::ElementType::Position3D>() = { side,side,side };// 7
			vertices[8].Attr<MP::ElementType::Position3D>() = { -side,-side,-side };// 8 left side
			vertices[9].Attr<MP::ElementType::Position3D>() = { -side,side,-side };// 9
			vertices[10].Attr<MP::ElementType::Position3D>() = { -side,-side,side };// 10
			vertices[11].Attr<MP::ElementType::Position3D>() = { -side,side,side };// 11
			vertices[12].Attr<MP::ElementType::Position3D>() = { side,-side,-side };// 12 right side
			vertices[13].Attr<MP::ElementType::Position3D>() = { side,side,-side };// 13
			vertices[14].Attr<MP::ElementType::Position3D>() = { side,-side,side };// 14
			vertices[15].Attr<MP::ElementType::Position3D>() = { side,side,side };// 15
			vertices[16].Attr<MP::ElementType::Position3D>() = { -side,-side,-side };// 16 bottom side
			vertices[17].Attr<MP::ElementType::Position3D>() = { side,-side,-side };// 17
			vertices[18].Attr<MP::ElementType::Position3D>() = { -side,-side,side };// 18
			vertices[19].Attr<MP::ElementType::Position3D>() = { side,-side,side };// 19
			vertices[20].Attr<MP::ElementType::Position3D>() = { -side,side,-side };// 20 top side
			vertices[21].Attr<MP::ElementType::Position3D>() = { side,side,-side };// 21
			vertices[22].Attr<MP::ElementType::Position3D>() = { -side,side,side };// 22
			vertices[23].Attr<MP::ElementType::Position3D>() = { side,side,side };// 23

			return {
				std::move(vertices),
			{
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
				}
			};
		}
		static IndexedList MakeIndependentTextured()
		{
			auto itl = MakeIndependent(std::move(MP::VertexLayout{}.Append(MP::ElementType::Position3D).Append(MP::ElementType::Normal).Append(MP::ElementType::Texture2D)));

			itl._mVertices[0].tc = { 0.0f,0.0f };
			itl._mVertices[1].tc = { 1.0f,0.0f };
			itl._mVertices[2].tc = { 0.0f,1.0f };
			itl._mVertices[3].tc = { 1.0f,1.0f };
			itl._mVertices[4].tc = { 0.0f,0.0f };
			itl._mVertices[5].tc = { 1.0f,0.0f };
			itl._mVertices[6].tc = { 0.0f,1.0f };
			itl._mVertices[7].tc = { 1.0f,1.0f };
			itl._mVertices[8].tc = { 0.0f,0.0f };
			itl._mVertices[9].tc = { 1.0f,0.0f };
			itl._mVertices[10].tc = { 0.0f,1.0f };
			itl._mVertices[11].tc = { 1.0f,1.0f };
			itl._mVertices[12].tc = { 0.0f,0.0f };
			itl._mVertices[13].tc = { 1.0f,0.0f };
			itl._mVertices[14].tc = { 0.0f,1.0f };
			itl._mVertices[15].tc = { 1.0f,1.0f };
			itl._mVertices[16].tc = { 0.0f,0.0f };
			itl._mVertices[17].tc = { 1.0f,0.0f };
			itl._mVertices[18].tc = { 0.0f,1.0f };
			itl._mVertices[19].tc = { 1.0f,1.0f };
			itl._mVertices[20].tc = { 0.0f,0.0f };
			itl._mVertices[21].tc = { 1.0f,0.0f };
			itl._mVertices[22].tc = { 0.0f,1.0f };
			itl._mVertices[23].tc = { 1.0f,1.0f };

			return itl;
		}
	};
}