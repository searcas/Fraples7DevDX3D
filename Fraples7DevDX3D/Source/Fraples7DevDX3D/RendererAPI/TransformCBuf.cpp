#include "TransformCBuf.h"
namespace FraplesDev
{

	TransformCBuf::TransformCBuf(Graphics& gfx, const Renderer& parent, UINT slot )
		:parent(parent)
	{
		if (!_spVcbuf)
		{
			_spVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuf::Bind(Graphics& gfx) noexcept
	{
		const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
		const Transforms transforms =
		{
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView *  gfx.GetProjection())
		};
		_spVcbuf->Update(gfx, transforms);
		_spVcbuf->Bind(gfx);
	}
	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::_spVcbuf;
}