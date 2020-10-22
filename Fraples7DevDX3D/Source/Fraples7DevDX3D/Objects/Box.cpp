#include "Box.h"
#include "../RendererAPI/GFXContextBase.h"
#include "Cube.h"


namespace FraplesDev
{
	Box::Box(Graphics& gfx, std::mt19937& rng, 
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material) 
		: BaseObject(gfx,rng,adist,ddist,odist,rdist)
	{
		if (!IsStaticInitialized())
		{
			struct Vertex
			{
				DirectX::XMFLOAT3 pos;
				DirectX::XMFLOAT3 normal;
			};

			auto model = Cube::MakeIndependent<Vertex>();
			model.SetNormalsIndependentFlat();
			AddStaticBind(std::make_unique<VertexBuffer>(gfx, model._mVertices));

			auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();


			AddStaticBind(std::move(pvs));
			AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

			AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model._mIndices));


			const std::vector<D3D11_INPUT_ELEMENT_DESC>IelementDesc =
			{
				{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			AddStaticBind(std::make_unique<InputLayout>(gfx, IelementDesc, pvsbc));
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		}
		else
		{
			SetIndexFromStatic();
		}
		AddBind(std::make_unique<TransformCBuf>(gfx, *this));
		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color;
			float specularIntentsity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		}colorConstant;
		colorConstant.color = material;
		AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConstant, 1u));
		DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));

	}


	const DirectX::XMMATRIX Box::GetTransformXM()const noexcept
	{
		return std::move(DirectX::XMLoadFloat3x3(&mt) *
			BaseObject::GetTransformXM());
	}
}
