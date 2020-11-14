#include "TexturePreprocessor.h"
#include "RendererAPI/Mesh/Model.h"
#include "Core/Math/Math.h"
#include <sstream>
#include <filesystem>
#include "assimp/Importer.hpp"
#include "RendererAPI/Mesh/ModelException.h"

namespace FraplesDev
{
	namespace cmd
	{

		template<typename F>
		inline void TexturePreprocessor::TransformSurface(Surface& surface, F&& func)
		{
			const auto width = surface.GetWidth();
			const auto height = surface.GetHeight();
			for (unsigned int y = 0; y < height; y++)
			{
				for (unsigned x = 0; x < width; x++)
				{
					const auto n = ColorToVector(surface.GetPixel(x, y));
					surface.PutPixel(x, y, VectorToColor(func(n, x, y)));
				}
			}
		}
		template<typename F>
		inline void TexturePreprocessor::TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func)
		{
			auto surface = Surface::FromFile(pathIn);

			TransformSurface(surface, func);
			surface.Save(pathOut);
		}
		void TexturePreprocessor::FlipYAllNormalMapsInObj(const std::string& objPath)
		{
			const auto rootPath = std::filesystem::path{ objPath }.parent_path().string() + "\\";

			//load scene from .obj file to get our list of normal maps in the materials
			Assimp::Importer imp;
			const auto pScene = imp.ReadFile(objPath.c_str(), 0u);

			if (pScene ==nullptr)
			{
				throw ModelException(__FILE__, __LINE__, imp.GetErrorString());
			}


			//loop trough materials and process any normal maps

			for (auto i = 0u; i < pScene->mNumMaterials; i++)
			{
				const auto& mat = *pScene->mMaterials[i];
				aiString texFileName;
				if (mat.GetTexture(aiTextureType_NORMALS,0,&texFileName)==aiReturn_SUCCESS)
				{
					const auto path = rootPath + texFileName.C_Str();
					FlipYNormalMap(path, path);
				}
			}
		}
		void TexturePreprocessor::FlipYNormalMap(const std::string& in, const std::string& out)
		{
			const auto flipY = DirectX::XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
			const auto processNormal = [flipY](DirectX::FXMVECTOR n, int x, int y) ->DirectX::XMVECTOR
			{
				return DirectX::XMVectorMultiply(n, flipY);
			};
			TransformFile(in, out, processNormal);
		}

		void TexturePreprocessor::ValidateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax)
		{
			OutputDebugStringA(("Validating normal map[" + pathIn + "]\n").c_str());

			//function for processing each normal in texture

			auto sum = DirectX::XMVectorZero();
			const auto ProcessNormal = [thresholdMin, thresholdMax, &sum](DirectX::FXMVECTOR n, int x, int y) ->DirectX::XMVECTOR
			{
				const float len = DirectX::XMVectorGetX(DirectX::XMVector3Length(n));
				const float z = DirectX::XMVectorGetZ(n);
				if (len < thresholdMin || len > thresholdMax)
				{
					DirectX::XMFLOAT3 vec;
					DirectX::XMStoreFloat3(&vec, n);
					std::ostringstream oss;
					oss << "Bad Normal length: " << len << " at: (" << x << ", " << y << ") normal: (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
					OutputDebugStringA(oss.str().c_str());
				}
				if (z < 0.0f)
				{
					DirectX::XMFLOAT3 vec;
					DirectX::XMStoreFloat3(&vec, n);
					std::ostringstream oss;
					oss << "Bad Normal Z direction at: (" << x << ", " << y << ") normal: " << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
					OutputDebugStringA(oss.str().c_str());
				}
				sum = DirectX::XMVectorAdd(sum, n);
				return n;
			};
			//execute the validation for each texel
			auto surf = Surface::FromFile(pathIn);
			TransformSurface(surf, ProcessNormal);

			//output bias
			{
				DirectX::XMFLOAT2 sumv = {};
				DirectX::XMStoreFloat2(&sumv, sum);
				std::stringstream oss;
				oss << "Normal map biases: (" << sumv.x << "," << sumv.y << ")\n";
				OutputDebugStringA(oss.str().c_str());
			}
		}

		void TexturePreprocessor::MakeStripes(const std::string& pathOut, int size, int stripeWidth)
		{
			//make sure texture dimension is power of 2
			auto power = log2(size);
			assert(modf(power, &power) == 0.0);
			//make sure stripe width enables at least 2 stripes
			assert(stripeWidth < size / 2);

			Surface s(size, size);

			for (int y = 0; y < size; y++)
			{
				for (int x = 0; x < size; x++)
				{
					Surface::Color c = { 0,0,0 };
					if ((x / stripeWidth) %2==0)
					{
						c = { 255,255,255 };
					}
					s.PutPixel(x, y, c);
				}
			}
			s.Save(pathOut);
		}

	
		DirectX::XMVECTOR TexturePreprocessor::ColorToVector(Surface::Color color)noexcept
		{
			auto n = DirectX::XMVectorSet((float)color.GetR(), (float)color.GetG(), (float)color.GetB(),0.0f);
			const auto all255 = DirectX::XMVectorReplicate(2.0f / 255.0f);
			n = DirectX::XMVectorMultiply(n, all255);
			const auto all1 = DirectX::XMVectorReplicate(1.0f);
			n = DirectX::XMVectorSubtract(n, all1);
			return n;
		}
		Surface::Color TexturePreprocessor::VectorToColor(DirectX::FXMVECTOR n)
		{
			const auto all1 = DirectX::XMVectorReplicate(1.0f);
			DirectX::XMVECTOR nOut = DirectX::XMVectorAdd(n, all1);
			const auto all255 = DirectX::XMVectorReplicate(255.0f / 2.0f);

			nOut = DirectX::XMVectorMultiply(nOut, all255);

			DirectX::XMFLOAT3 floats;
			DirectX::XMStoreFloat3(&floats, nOut);

			return { 
			(unsigned char)round(floats.x),
			(unsigned char)round(floats.y),
			(unsigned char)round(floats.z)
			};

		}

	}
}