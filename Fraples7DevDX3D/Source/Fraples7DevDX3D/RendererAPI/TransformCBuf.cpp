#include "TransformCBuf.h"
namespace FraplesDev
{

	TransformCBuf::TransformCBuf(Graphics& gfx, UINT slot )
	{
		if (!_spVcbuf)
		{
			_spVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx)noexcept(!IS_DEBUG)
	{
		INFOMAN_NOHR(gfx);
		FPL_GFX_THROW_INFO_ONLY(UpdateBindImpl(gfx, GetTransforms(gfx)));
	}
	void TransformCBuf::InitializeParentReference(const Renderer& parent) noexcept
	{
		_mPparent = &parent;
	}
	std::unique_ptr<CloningContext> TransformCBuf::Clone() const noexcept
	{
		return std::make_unique<TransformCBuf>(*this);
	}
	void TransformCBuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept(!IS_DEBUG)
	{
		_spVcbuf->Update(gfx, tf);
		_spVcbuf->Bind(gfx);
	}
	TransformCBuf::Transforms TransformCBuf::GetTransforms(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		assert(_mPparent != nullptr);
		const auto modelView = _mPparent->GetTransformXM() * gfx.GetCamera();
		return
		{
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView * gfx.GetProjection())
		};

	}
	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::_spVcbuf;
}