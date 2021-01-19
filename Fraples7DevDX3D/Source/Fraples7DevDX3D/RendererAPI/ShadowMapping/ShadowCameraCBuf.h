#pragma once
#include "RendererAPI/GFXContext.h"
namespace FraplesDev
{
	class Camera;
	template<typename T>
	class VertexConstantBuffer;
	class ShadowCameraCBuf : public GfxContext
	{
	
	public:
		struct Transform
		{
			DirectX::XMMATRIX VieWProj;
		};
		ShadowCameraCBuf(Graphics& gfx, UINT slot = 3u);
		void Bind(Graphics& gfx)noexcept(!IS_DEBUG)override;
		void Update(Graphics& gfx);
		void SetCamera(const Camera* pCamera)noexcept;
	private:
		std::unique_ptr<VertexConstantBuffer<Transform>>_mVertexConstantBuffer;
		const Camera* _mCamera;
	
	};
}
