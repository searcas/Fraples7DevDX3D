#pragma once
#include "GraphicAPI/Graphics.h"
#include "RendererAPI/ConstantBuffers.h"
#include "SolidSphere.h"
#include "RendererAPI/RenderGraph/RenderGraph.h"



namespace FraplesDev
{
	class Camera;
	class PointLight
	{
	public:
		PointLight() = default;
		PointLight(Graphics& gfx, DirectX::XMFLOAT3 pos = { 10.0f,9.0f,2.5f }, float radius = 0.5f);
		void SpawnControlWindow()noexcept;
		void Reset()noexcept;
		void Submit(size_t channels) const noexcept(!IS_DEBUG);
		void LinkTechniques(RenderGraph&);
		std::shared_ptr<Camera>ShareCamera()const noexcept;
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
		PointLightCBuf _mHome;
		PointLightCBuf _mConstantBufferData;
		mutable SolidSphere _mMesh;
		mutable PixelConstantBuffer<PointLightCBuf>_mCbuf;
		std::shared_ptr<Camera>_mCamera;
	};

}