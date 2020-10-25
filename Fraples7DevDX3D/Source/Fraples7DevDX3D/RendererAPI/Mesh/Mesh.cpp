#include "Mesh.h"


namespace FraplesDev
{
	Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<GfxContext>>bindPtrs)
	{
		if (!IsStaticInitialized())
		{
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}
		for (auto& pb : bindPtrs)
		{
			if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>{pi});
				pb.release();
			}
			else
			{
				AddBind(std::move(pb));
			}
		}
		AddBind(std::make_unique<TransformCBuf>(gfx, *this));

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