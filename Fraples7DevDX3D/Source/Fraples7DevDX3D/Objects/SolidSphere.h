#pragma once
#include "../RendererAPI/RendererBase.h"

namespace FraplesDev
{
	class SolidSphere : public RendererBase<SolidSphere>
	{
	public:
		SolidSphere(Graphics& gfx, float radius);
		void Update(float dt)noexcept override;
		void SetPos(DirectX::XMFLOAT3 pos)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;

	private:
		DirectX::XMFLOAT3 _mPos = { 1.0f,1.0f,1.0f };
	};
}