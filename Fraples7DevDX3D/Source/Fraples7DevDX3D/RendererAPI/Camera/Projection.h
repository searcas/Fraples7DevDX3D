#pragma once
#include <DirectXMath.h>

namespace FraplesDev
{
	class Projection
	{
	public:
		Projection(float width,float height,float nearZ,float farZ);
		DirectX::XMMATRIX GetMatrix()const;
		void RenderWidgets();
	private:
		float _mWidth;
		float _mHeight;
		float _mNearZ;
		float _mFarZ;

	};
}