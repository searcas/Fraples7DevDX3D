#include "TransformCBuf.h"
namespace FraplesDev
{

	TransformCBuf::TransformCBuf(Graphics& gfx, const Renderer& parent)
		:parent(parent)
	{
		if (!_spVcbuf)
		{
			_spVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx) noexcept
	{
		_spVcbuf->Update(gfx, DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetCamera() * gfx.GetProjection()));
		_spVcbuf->Bind(gfx);
	}
	std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCBuf::_spVcbuf;
}