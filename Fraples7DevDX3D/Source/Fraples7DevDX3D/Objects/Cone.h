#pragma once
#include "../Core/Math/Math.h"
#include <DirectXMath.h>
#include "../Core/IndexedList.h"
#include "Core/MetaProgramming/Vertex.h"
namespace FraplesDev
{
	class Cone
	{
	public:
		static IndexedList MakeTesselated(int longDiv)
		{
			namespace dx = DirectX;
			assert(longDiv >= 3);

			const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
			const float longitudeAngle = 2.0f * PI / longDiv;

			// base vertices
			MP::VertexLayout vertex;
			for (int iLong = 0; iLong < longDiv; iLong++)
			{
				
				auto v = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				dx::XMStoreFloat3(&vertices.back().pos, v);
			}
			// the center
			vertices.emplace_back();
			vertices.back().pos = { 0.0f,0.0f,-1.0f };
			const auto iCenter = (unsigned short)(vertices.size() - 1);
			// the tip :darkness:
			vertices.emplace_back();
			vertices.back().pos = { 0.0f,0.0f,1.0f };
			const auto iTip = (unsigned short)(vertices.size() - 1);


			// base indices
			std::vector<unsigned short> indices;
			for (unsigned short iLong = 0; iLong < longDiv; iLong++)
			{
				indices.push_back(iCenter);
				indices.push_back((iLong + 1) % longDiv);
				indices.push_back(iLong);
			}

			// cone indices
			for (unsigned short iLong = 0; iLong < longDiv; iLong++)
			{
				indices.push_back(iLong);
				indices.push_back((iLong + 1) % longDiv);
				indices.push_back(iTip);
			}

			return { std::move(vertices),std::move(indices) };
		}
		static IndexedList MakeTessellatedIndependentFaces(int longDiv)
		{
			assert(longDiv >= 3);
			const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
			const float longitudeAngle = 2.0f * PI / longDiv;

			std::vector<V>vertices;

			const auto iCone = static_cast<unsigned short>(vertices.size());

			for (int iLong = 0; iLong < longDiv; iLong++)
			{
				const float thetas[] = { longitudeAngle * iLong, 
					longitudeAngle * (((iLong + 1) == longDiv) ? 0 : (iLong + 1)) };
				vertices.emplace_back();
				vertices.back().pos = { 0.0f,0.0f,1.0f };
				for (auto theta : thetas)
				{
					vertices.emplace_back();
					const auto v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(theta));
					DirectX::XMStoreFloat3(&vertices.back().pos, v);
				}
			}
			const auto  iBaseCenter = static_cast<unsigned short>(vertices.size());
			vertices.emplace_back();
			vertices.back().pos = { 0.0f,0.0f,-1.0f };
			const auto iBaseEdge = static_cast<unsigned short>(vertices.size());
			for (int iLong = 0; iLong < longDiv; iLong++)
			{
				vertices.emplace_back();
				auto v = DirectX::XMVector3Transform(base, 
					DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
				DirectX::XMStoreFloat3(&vertices.back().pos, v);
			}
			std::vector<unsigned short>indices;

			//cone indices

			for (unsigned short i = 0; i < longDiv * 3; i++)
			{
				indices.push_back(i + iCone);
			}
			//base indices
			for (unsigned short iLong = 0; iLong < longDiv; iLong++)
			{
				indices.push_back(iBaseCenter);
				indices.push_back((iLong + 1) % longDiv + iBaseEdge);
				indices.push_back(iLong + iBaseEdge);

			}
			return { std::move(vertices),std::move(indices) };
		}
		template<class V>
		static IndexedList<V> Make()
		{
			return MakeTesselated<V>(24);
		}

	};
}