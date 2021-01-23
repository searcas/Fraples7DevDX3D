#pragma once
#include <DirectXMath.h>
#include <initializer_list>
#include <optional>
#include "../Core/IndexedList.h"
namespace FraplesDev
{


	class Cube
	{
	public:
		static IndexedList Make(std::optional<MP::VertexLayout> layout = {})
		{
			if (!layout)
			{
				layout = MP::VertexLayout{};
				layout->Append(MP::ElementType::Position3D);
			}
			constexpr float side = 1.0f / 2.0f;

			MP::VertexBuffer vertices(std::move(*layout),8u);

			vertices[0].Attr<MP::ElementType::Position3D>() = { -side,-side,-side };
			vertices[1].Attr<MP::ElementType::Position3D>() = {  side,-side,-side };
			vertices[2].Attr<MP::ElementType::Position3D>() = { -side, side,-side };
			vertices[3].Attr<MP::ElementType::Position3D>() = {  side, side,-side };
			vertices[4].Attr<MP::ElementType::Position3D>() = { -side,-side,-side };
			vertices[5].Attr<MP::ElementType::Position3D>() = {  side,-side, side };
			vertices[6].Attr<MP::ElementType::Position3D>() = { -side, side, side };
			vertices[7].Attr<MP::ElementType::Position3D>() = {  side, side, side };

			return { std::move(vertices),
			{
					0,2,1, 2,3,1,
					1,3,5, 3,7,5,
					2,6,3, 3,6,7,
					4,5,7, 4,7,6,
					0,4,2, 2,4,6,
					0,1,4, 1,5,4
			} };
		}
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

			itl._mVertices[0].Attr<MP::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._mVertices[1].Attr<MP::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._mVertices[2].Attr<MP::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._mVertices[3].Attr<MP::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._mVertices[4].Attr<MP::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._mVertices[5].Attr<MP::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._mVertices[6].Attr<MP::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._mVertices[7].Attr<MP::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._mVertices[8].Attr<MP::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._mVertices[9].Attr<MP::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._mVertices[10].Attr<MP::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._mVertices[11].Attr<MP::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._mVertices[12].Attr<MP::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._mVertices[13].Attr<MP::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._mVertices[14].Attr<MP::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._mVertices[15].Attr<MP::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._mVertices[16].Attr<MP::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._mVertices[17].Attr<MP::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._mVertices[18].Attr<MP::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._mVertices[19].Attr<MP::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._mVertices[20].Attr<MP::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._mVertices[21].Attr<MP::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._mVertices[22].Attr<MP::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._mVertices[23].Attr<MP::ElementType::Texture2D>() = { 1.0f,1.0f };

			return itl;
		}
	};
}