#pragma once

#include "RendererAPI/Renderer.h"
namespace FraplesDev
{
	class CameraProjection : public Renderer
	{
	public:
		CameraProjection(Graphics& gfx);
		void SetPosition(DirectX::XMFLOAT3 pos)noexcept;
		void SetRotation(DirectX::XMFLOAT3 pos)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;
	private:
		DirectX::XMFLOAT3 _mPos = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 _mRot = { 0.0f,0.0f,0.0f };
	};


}