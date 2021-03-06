#include "ShadowCameraCBuf.h"
#include "RendererAPI/ConstantBuffers.h"
#include "RendererAPI/Camera/Camera.h"
namespace FraplesDev
{
	ShadowCameraCBuf::ShadowCameraCBuf(Graphics& gfx, UINT slot)
		:_mVertexConstantBuffer{ std::make_unique<VertexConstantBuffer<Transform>>(gfx,slot) }
	{
		_mCamera = nullptr;
	}
	void ShadowCameraCBuf::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		_mVertexConstantBuffer->Bind(gfx);
	}
	void ShadowCameraCBuf::Update(Graphics& gfx)
	{
		const auto pos = _mCamera->GetPos();
		const Transform T{ DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(-pos.x,-pos.y,-pos.z)) };
		_mVertexConstantBuffer->Update(gfx, T);
	}
	void ShadowCameraCBuf::SetCamera(const Camera* pCamera) noexcept
	{
		_mCamera = pCamera;
	}
}