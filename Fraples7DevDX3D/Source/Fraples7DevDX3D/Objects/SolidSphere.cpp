#include "SolidSphere.h"
#include "../Objects/Sphere.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "../RendererAPI/GFXContextBase.h"

namespace FraplesDev
{
	SolidSphere::SolidSphere(Graphics& gfx, float radius)
	{
		
		if (!IsStaticInitialized())
		{
			struct Vertex
			{
				DirectX::XMFLOAT3 pos;
			};

			auto model = Sphere::Make<Vertex>();

			model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
			AddBind(std::make_unique<VertexBuffer>(gfx, model._mVertices));
			AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model._mIndices));
			auto pvs = std::make_unique<VertexShader>(gfx, L"SolidVS.cso");
			auto pvsbyte = pvs->GetBytecode();

			AddStaticBind(std::move(pvs));

			AddStaticBind(std::make_unique<PixelShader>(gfx, L"SolidPS.cso"));

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
				float padding;
			}colorConst;

			AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));
			const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
			{
				{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			};

			AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbyte));

			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		}
		else
		{
			SetIndexFromStatic();
		}
		AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	}

	void SolidSphere::Update(float diff)noexcept
	{

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