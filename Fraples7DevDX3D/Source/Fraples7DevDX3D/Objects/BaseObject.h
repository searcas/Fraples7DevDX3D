#pragma once
#include <random>
#include "../RendererAPI/RendererBase.h"
#include "../Core/Math/Math.h"
namespace FraplesDev
{
	class BaseObject : public Renderer
	{
	public:
		BaseObject(Graphics& gfx, std::mt19937& rng,
			std::uniform_real_distribution<float>& adist,
			std::uniform_real_distribution<float>& ddist,
			std::uniform_real_distribution<float>& odist,
			std::uniform_real_distribution<float>& rdist)
			:
			r(rdist(rng)),
			droll(ddist(rng)),
			dpitch(ddist(rng)),
			dyaw(ddist(rng)),
			dphi(odist(rng)),
			dtheta(odist(rng)),
			dchi(odist(rng)),
			chi(adist(rng)),
			theta(adist(rng)),
			phi(adist(rng))
		{

		}
	
		const DirectX::XMMATRIX GetTransformXM() const noexcept
		{
			return std::move(
				DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
				DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
				DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi));
		}
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
	};


}
