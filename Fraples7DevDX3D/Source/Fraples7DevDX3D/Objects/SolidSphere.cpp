#include "SolidSphere.h"
#include "Objects/Sphere.h"
#include "Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "RendererAPI/GFXContextBase.h"
#include "Core/MetaProgramming/Vertex.h"

namespace FraplesDev
{
	SolidSphere::SolidSphere(Graphics& gfx, float radius)
	{
			auto model = Sphere::Make();
			model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
			const auto geometryTag = "$sphere." + std::to_string(radius);
			AddBind(VertexBuffer::Resolve(gfx, geometryTag,model._mVertices));
			AddBind(IndexBuffer::Resolve(gfx, geometryTag,model._mIndices));

			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			auto pvsbyte = pvs->GetBytecode();

			AddBind(std::move(pvs));

			AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
				float padding;
			}colorConst;

			AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst,1u));
			
			AddBind(InputLayout::Resolve(gfx, model._mVertices.GetLayout(), pvsbyte));


			AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			AddBind(std::make_shared<TransformCBuf>(gfx, *this));
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