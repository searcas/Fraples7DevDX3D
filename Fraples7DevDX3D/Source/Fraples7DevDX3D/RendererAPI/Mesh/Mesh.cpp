#include "Mesh.h"


namespace FraplesDev
{
	Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept(!IS_DEBUG)
		:Renderer(gfx, mat, mesh)
	{
	}
	void Mesh::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&_mTransform, accumulatedTransform);
		Renderer::Submit(frame);
	}
	const DirectX::XMMATRIX Mesh::GetTransformXM()const noexcept 
	{
		return DirectX::XMLoadFloat4x4(&_mTransform);
	}
}