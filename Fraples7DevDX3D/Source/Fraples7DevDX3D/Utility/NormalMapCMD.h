#pragma once
#include "Core/Surface.h"
#include "Core/Math/Math.h"
#include <string>
#include <DirectXMath.h>


namespace FraplesDev
{
	//commands
	namespace cmd
	{
		class NormalMap
		{
		public:
			static void RotateXAxis180(const std::string& pathIn, std::string pathOut)
			{
				const auto rotation = DirectX::XMMatrixRotationX(PI);
				auto sIn = Surface::FromFile(pathIn);

				const auto nPixels = sIn.GetWidth() * sIn.GetHeight();
				const auto pBegin = sIn.GetBufferPtr();
				const auto pEnd = sIn.GetBufferPtrConst() + nPixels;

				for (auto pCurrent = pBegin; pCurrent < pEnd; pCurrent++)
				{
					auto n = ColorToVector(*pCurrent);
					n = DirectX::XMVector3Transform(n, rotation);
					*pCurrent = VectorToColor(n);
				}
				sIn.Save(pathOut);
			}
			static void RotateXAxis180(const std::string& pathIn)
			{
				RotateXAxis180(pathIn, pathIn);
			}
			static DirectX::XMVECTOR ColorToVector(Surface::Color c)
			{
				auto n = DirectX::XMVectorSet((float)c.GetR(), (float)c.GetG(), (float)c.GetB(), 0.0f);

				const auto all255 = DirectX::XMVectorReplicate(2.0f / 255.0f);

				n = DirectX::XMVectorMultiply(n, all255);
				const auto all1 = DirectX::XMVectorReplicate(1.0f);

				n = DirectX::XMVectorSubtract(n, all1);

				return n;
			}
			static Surface::Color VectorToColor(DirectX::FXMVECTOR n)
			{
				const auto all1 = DirectX::XMVectorReplicate(1.0f);

				DirectX::XMVECTOR nOut = DirectX::XMVectorAdd(n, all1);
				const auto all255 = DirectX::XMVectorReplicate(255.0f / 2.0f);

				nOut = DirectX::XMVectorMultiply(nOut, all255);
				DirectX::XMFLOAT3 floats;
				DirectX::XMStoreFloat3(&floats, nOut);

				return{ (unsigned char)round(floats.x),(unsigned char)round(floats.y), (unsigned char)round(floats.z) };
			}
		private:

		};
	}
}