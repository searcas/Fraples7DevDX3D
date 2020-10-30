#pragma once
#include "RendererAPI/Renderer.h"
namespace FraplesDev
{
	class BaseCube : public Renderer
	{
	public:
		BaseCube(Graphics& gfx, float size);
		void SetPos(DirectX::XMFLOAT3 pos)noexcept;
		void SetRotation(float roll, float pitch, float yaw)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;
		void SpawnControlWindow(Graphics& gfx)noexcept;
	private:
		struct PSMaterialConstant
		{
			float specularIntensity = 0.1f;
			float specularPower = 19.1f;
			BOOL normalMappingEnabled = TRUE;
			float padding[1] = {};
		}pmc;
		DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
	private:

	};

	
	
	

	
	
	
}