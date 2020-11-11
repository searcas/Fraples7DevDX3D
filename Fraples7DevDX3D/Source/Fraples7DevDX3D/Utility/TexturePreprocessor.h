#pragma once
#include "Core/Surface.h"
#include <string>
#include <DirectXMath.h>


namespace FraplesDev
{
	namespace cmd
	{
		class TexturePreprocessor
		{
		public:
			static void FlipYAllNormalMapsInObj(const std::string& objPath);
			static void FlipYNormalMap(const std::string& in, const std::string& out);
			static void ValidateNormalMap(const std::string& pathIn, float threshHoldMin, float threshHoldMax);
			static void MakeStripes(const std::string& pathOut, int size, int stripeWidth);
		private:
			template<typename F>
			static void TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func);
			template<typename F>
			static void TransformSurface(Surface& surf, F&& func);
			static DirectX::XMVECTOR ColorToVector(Surface::Color color)noexcept;
			static Surface::Color VectorToColor(DirectX::FXMVECTOR n);
		};
	}
}