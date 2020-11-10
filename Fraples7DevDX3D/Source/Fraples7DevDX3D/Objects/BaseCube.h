#pragma once
#include "RendererAPI/Renderer.h"
#include "RendererAPI/GFXContext.h"
#include "RendererAPI/IndexBuffer.h"
namespace FraplesDev
{
	class BaseCube : public Renderer
	{
	public:
		BaseCube(Graphics& gfx, float size);
		void SetPos(DirectX::XMFLOAT3 pos)noexcept;
		void SetRotation(float roll, float pitch, float yaw)noexcept;
		const DirectX::XMMATRIX GetTransformXM()const noexcept override;
		void SpawnControlWindow(Graphics& gfx, const char* name)noexcept;

		void RenderOutline(Graphics& gfx) noexcept(!IS_DEBUG)
		{
			outLining = true;
			for (auto& i : outLineEffect)
			{
				i->Bind(gfx);
			}
			gfx.RenderIndexed(QueryBindable<IndexBuffer>()->GetCount());
			outLining = false;
		}
	private:
		struct PSMaterialConstant
		{
			float specularIntensity = 0.1f;
			float specularPower = 20.0f;
			BOOL normalMappingEnabled = TRUE;
			float padding[1] = {};
		}pmc;
		DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		bool outLining = false;
	private:
		std::vector<std::shared_ptr<GfxContext>>outLineEffect;
	};

	
	
	

	
	
	
}