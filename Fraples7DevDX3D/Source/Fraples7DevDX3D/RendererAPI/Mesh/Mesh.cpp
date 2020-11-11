#include "Mesh.h"


namespace FraplesDev
{
	
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