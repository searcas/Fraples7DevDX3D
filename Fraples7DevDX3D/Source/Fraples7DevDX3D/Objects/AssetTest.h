#pragma once

#include "AssetTest.h"
#include "BaseObject.h"
#include "RendererAPI\ConstantBuffers.h"

namespace FraplesDev
{
	class Asset : public BaseObject
	{
	public:
		Asset(Graphics& gfx, std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			DirectX::XMFLOAT3 material, float scale);
	private:

	};



}