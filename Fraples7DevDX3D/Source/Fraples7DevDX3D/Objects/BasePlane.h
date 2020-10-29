#pragma once
#include "RendererAPI/Renderer.h"

namespace FraplesDev
{
	class BasePlane : public Renderer
	{
	public:
		BasePlane(Graphics& gfx, float size);
		void SetPosXYZ(DirectX::XMFLOAT3 pos)	noexcept;
		void SetRotationRPY(float roll, float pitch, float yaw)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;
	private:
		DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	};
}