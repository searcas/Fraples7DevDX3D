#include "TransformCBufDual.h"
namespace FraplesDev
{
	TransformCbufDual::TransformCbufDual(Graphics& gfx, const Renderer& parent, UINT slotV, UINT slotP)
		: TransformCBuf(gfx,parent,slotV)
	{
		if (!_sPpCbuf)
		{
			_sPpCbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
		}
	}
	void TransformCbufDual::Bind(Graphics& gfx) noexcept
	{
		const auto tf = GetTransforms(gfx);
		TransformCBuf::UpdateBindImpl(gfx, tf);
		UpdateBindImpl(gfx, tf);
	}
	void TransformCbufDual::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		_sPpCbuf->Update(gfx, tf);
		_sPpCbuf->Bind(gfx);
	}
	 std::unique_ptr<PixelConstantBuffer<TransformCbufDual::Transforms>> TransformCbufDual::_sPpCbuf;

}