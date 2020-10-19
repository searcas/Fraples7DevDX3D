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
		std::uniform_real_distribution<float>& bdist)
		:
		r(rdist(rng)),
		droll(ddist(rng)),
		dpitch(ddist(rng)),
		dyaw(ddist(rng)),
		dphi(odist(rng)),
		dtheta(odist(rng)),
		dchi(odist(rng)),
		chi(adist(rng)),
		theta(adist(rng)),
		phi(adist(rng))
	{
		if (!IsStaticInitialized())
		{
			struct Vertex
			{
				DirectX::XMFLOAT3 pos;
			};

			const auto model = Cube::Make<Vertex>();
			AddStaticBind(std::make_unique<VertexBuffer>(gfx, model._mVertices));

			auto pvs = std::make_unique<VertexShader>(gfx, L"ColorIndexVS.cso");

			auto pvsbc = pvs->GetBytecode();
			AddStaticBind(std::move(pvs));

			AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPS.cso"));

			AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model._mIndices));
			struct PixelShaderConstants
			{
				struct
				{
					float r, g, b, a;
				}face_colors[8];
			};


			const PixelShaderConstants cb2 = {
				{

					{ 1.0f, 1.0f, 1.0f },
					{ 1.0f, 0.0f, 0.0f },
					{ 0.0f, 1.0f, 0.0f },
					{ 1.0f, 1.0f, 0.0f },
					{ 0.0f, 0.0f, 1.0f },
					{ 1.0f, 0.0f, 1.0f },
					{ 0.0f, 1.0f, 1.0f },
					{ 0.0f, 0.0f, 0.0f },
				}
			};
			AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb2));

			const std::vector<D3D11_INPUT_ELEMENT_DESC>IelementDesc =
			{
				{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			AddStaticBind(std::make_unique<InputLayout>(gfx, IelementDesc, pvsbc));
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		}
		else
		{
			SetIndexFromStatic();
		}
		AddBind(std::make_unique<TransformCBuf>(gfx, *this));
		DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));

	}
	void Box::Update(float diff)noexcept
	{
		roll += droll * diff;
		pitch += dpitch * diff;
		yaw += dyaw * diff;
		theta += dtheta * diff;
		phi += dphi * diff;
		chi += dchi * diff;
	}

	const DirectX::XMMATRIX Box::GetTransformXM()const noexcept
	{
		return std::move(DirectX::XMLoadFloat3x3(&mt) *
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi));
	}
}
