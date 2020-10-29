#include "Mesh.h"


namespace FraplesDev
{
	Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<GfxContext>>bindPtrs)
	{
		AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		for (auto& pb : bindPtrs)
		{
			AddBind(std::move(pb));
		}
		AddBind(std::make_shared<TransformCBuf>(gfx, *this));
	}
	void Mesh::Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&_mTransform, accumulatedTransform);
		Renderer::Render(gfx);
	}
	const DirectX::XMMATRIX Mesh::GetTransformXM()const noexcept 
	{
		return DirectX::XMLoadFloat4x4(&_mTransform);
	}
}