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

	void TransformCBuf::Bind(Graphics& gfx) noexcept
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
		
	}
	void TransformCBuf::InitializeParentReference(const Renderer& parent) noexcept
	{
		_mPparent = &parent;
	}
	void TransformCBuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		_spVcbuf->Update(gfx, tf);
		_spVcbuf->Bind(gfx);
	}
	TransformCBuf::Transforms TransformCBuf::GetTransforms(Graphics& gfx) noexcept
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