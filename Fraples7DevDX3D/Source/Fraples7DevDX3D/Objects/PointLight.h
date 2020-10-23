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
		void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
	private:
		struct PointLightCBuf
		{
		alignas(16)	DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;

			float diffuseIntensity;
			float attConst;
			float attLin;
			float attQuad;
		};


	private:
		PointLightCBuf _mConstantBufferData;
		mutable SolidSphere _mMesh;
		mutable PixelConstantBuffer<PointLightCBuf>_mCbuf;
	};

}