#pragma once
#include <DirectXMath.h>
#include "ViewFrustum.h"
namespace FraplesDev
{
	class Projection
	{
	public:
		Projection(Graphics& gfx, float width,float height,float nearZ,float farZ);
		DirectX::XMMATRIX GetMatrix()const;
		void RenderWidgets(Graphics& gfx);
		void SetPosition(DirectX::XMFLOAT3);
		void SetRotation(DirectX::XMFLOAT3);
		void Submit()const;
		void LinkTechniques(RenderGraph& rg);
	private:
		float _mWidth;
		float _mHeight;
		float _mNearZ;
		float _mFarZ;
		Frustum _mFrust;
	};
}