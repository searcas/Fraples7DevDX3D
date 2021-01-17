#pragma once
#include "RendererAPI/Renderer.h"

namespace FraplesDev
{
	class Frustum : public Renderer
	{
	public:
		Frustum(Graphics& gfx, float width, float height, float nearZ, float farZ);
		void SetVertices(Graphics& gfx, float width, float height, float nearZ, float farZ);
		void SetPosition(DirectX::XMFLOAT3 pos)noexcept;
		void SetRotation(DirectX::XMFLOAT3 rot)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;

	private:
		DirectX::XMFLOAT3 _mPosition = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 _mRotation = { 0.0f,0.0f,0.0f };

	};
}
