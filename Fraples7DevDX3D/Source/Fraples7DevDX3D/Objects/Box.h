#pragma once
#include "../GraphicAPI/Graphics.h"
#include "../RendererAPI/ConstantBuffers.h"
#include "BaseObject.h"
#include <random>
namespace FraplesDev
{
	class Box : public BaseObject<Box>
	{
	public:
		Box(Graphics& gfx, std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			std::uniform_real_distribution<float>& bdist,
			DirectX::XMFLOAT3 material);
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;
		DirectX::XMFLOAT3X3 mt;
		bool SpawnControlWindow(int id, Graphics& gfx)noexcept;
	private:
		void SyncMaterial(Graphics& gfx)noexcept(!IS_DEBUG);
	private:
		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color;
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		}materialConstants;
		using MaterialCbuf = PixelConstantBuffer<PSMaterialConstant>;
	private:
		DirectX::XMFLOAT3 _mModelTransform;
	};

	
}