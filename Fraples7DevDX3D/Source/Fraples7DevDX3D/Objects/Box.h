#pragma once
#include "../RendererAPI/RendererBase.h"
#include <random>
namespace FraplesDev
{
	class Box : public RendererBase<Box>
	{
	public:
		Box(Graphics& gfx, std::mt19937& rng, 
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist,
			std::uniform_real_distribution<float>& bdist);
		void Update(float diff)noexcept override;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;
	protected:
		//positional
		float r;
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float theta;
		float phi;
		float chi;
		//speed (Delta/s)
		float droll;
		float dpitch;
		float dyaw;
		float dtheta;
		float dphi;
		float dchi;
		DirectX::XMFLOAT3X3 mt;

	};

	
}