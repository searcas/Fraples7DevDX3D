#pragma once
#include "RendererAPI/Renderer.h"

namespace FraplesDev
{
	class BasePlane : public Renderer
	{
	public:
		BasePlane(Graphics& gfx, float size, DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,0.0f });
		void SetPosXYZ(DirectX::XMFLOAT3 pos)	noexcept;
		void SetRotationRPY(float roll, float pitch, float yaw)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;
		void SpawnControlWindow(Graphics& gfx,const std::string& name)noexcept;
	private:
		
		struct PSMaterialConstant
		{
			DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,0.0f };
			float specularIntensity = 0.18f;
			float specularPower = 18.0f;
			BOOL normalMappingEnabled = TRUE;
			float padding[1] = {};
		}pmc;
		DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;

	
	};
}