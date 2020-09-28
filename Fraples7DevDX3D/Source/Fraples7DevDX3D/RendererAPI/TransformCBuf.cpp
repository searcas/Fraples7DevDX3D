#include "TransformCBuf.h"

FraplesDev::TransformCBuf::TransformCBuf(Graphics& gfx, const Renderer& parent)
	:vcbuf(gfx),parent(parent)
{

}

void FraplesDev::TransformCBuf::Bind(Graphics& gfx) noexcept
{
	vcbuf.Update(gfx, DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection()));
	vcbuf.Bind(gfx);
}
