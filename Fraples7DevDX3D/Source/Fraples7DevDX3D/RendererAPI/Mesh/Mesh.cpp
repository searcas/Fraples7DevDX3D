#include "Mesh.h"


namespace FraplesDev
{
	Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh,float scale) noexcept(!IS_DEBUG)
		: Renderer(gfx, mat, mesh,scale)
	{
		_mTransform = {};
	}
	void Mesh::Submit(DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&_mTransform, accumulatedTransform);
		Renderer::Submit();
	}
	const DirectX::XMMATRIX Mesh::GetTransformXM()const noexcept 
	{
		return DirectX::XMLoadFloat4x4(&_mTransform);
	}
}