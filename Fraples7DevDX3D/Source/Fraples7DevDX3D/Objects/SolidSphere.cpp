#include "SolidSphere.h"
#include "../Objects/Sphere.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "../RendererAPI/GFXContextBase.h"
#include "Core/MetaProgramming/Vertex.h"

namespace FraplesDev
{
	SolidSphere::SolidSphere(Graphics& gfx, float radius)
	{
			auto model = Sphere::Make();
			model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
			AddBind(std::make_shared<VertexBuffer>(gfx, model._mVertices));
			AddBind(std::make_shared<IndexBuffer>(gfx, model._mIndices));

			auto pvs = std::make_shared<VertexShader>(gfx, "SolidVS.cso");
			auto pvsbyte = pvs->GetBytecode();

			AddBind(std::move(pvs));

			AddBind(std::make_unique<PixelShader>(gfx, "SolidPS.cso"));

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
				float padding;
			}colorConst;

			AddBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));
			
			AddBind(std::make_shared<InputLayout>(gfx, model._mVertices.GetLayout(), pvsbyte));


			AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	}


	void SolidSphere::SetPos(DirectX::XMFLOAT3 pos)noexcept
	{
		_mPos = pos;
	}
	const DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixTranslation(_mPos.x, _mPos.y, _mPos.z);
	}
}