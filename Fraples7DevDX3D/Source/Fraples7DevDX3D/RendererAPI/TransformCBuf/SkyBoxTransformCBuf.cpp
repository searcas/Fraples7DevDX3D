#include "SkyBoxTransformCBuf.h"
#include "Core/Common/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/ConstantBuffers.h"
namespace FraplesDev
{
	SkyBoxTransformCBuf::SkyBoxTransformCBuf(Graphics& gfx, UINT slot)
		:_mVertexConstBuffer{std::make_unique<VertexConstantBuffer<Transforms>>(gfx,slot)}
	{
	}
	void SkyBoxTransformCBuf::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(UpdateBindImpl(gfx, GetTransforms(gfx)));
	}
	void SkyBoxTransformCBuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept(!IS_DEBUG)
	{
		_mVertexConstBuffer->Update(gfx, tf);
		_mVertexConstBuffer->Bind(gfx);
	}
	SkyBoxTransformCBuf::Transforms SkyBoxTransformCBuf::GetTransforms(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		return { DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection()) };
	}
}