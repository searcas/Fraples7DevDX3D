#pragma once
#include "../RendererAPI/renderer.h"

namespace FraplesDev
{
	class SolidSphere : public Renderer
	{
	public:
		SolidSphere(Graphics& gfx, float radius);
		void SetPos(DirectX::XMFLOAT3 pos)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;

	private:
		DirectX::XMFLOAT3 _mPos = { 1.0f,1.0f,1.0f };
	};
}