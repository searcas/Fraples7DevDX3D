#pragma once
#include "../GraphicAPI/Graphics.h"
#include "SolidSphere.h"
#include "../RendererAPI/ConstantBuffers.h"



namespace FraplesDev
{
	class PointLight
	{
	public:
		PointLight() = default;
		PointLight(Graphics& gfx, float radius = 0.5f);
		void SpawnControlWindow()noexcept;
		void Reset()noexcept;
		void Render(Graphics& gfx) const noexcept(!IS_DEBUG);
		void Bind(Graphics& gfx) const noexcept;
		~PointLight()= default;
	private:
		struct PointLightCBuf
		{
			DirectX::XMFLOAT3 pos;
			float padding;
		};

	private:
		DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
		mutable SolidSphere _mMesh;
		mutable PixelConstantBuffer<PointLightCBuf>_mCbuf;
	};

}