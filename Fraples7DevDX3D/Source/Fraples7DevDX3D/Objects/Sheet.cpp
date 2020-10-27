#include "Sheet.h"
#include "../RendererAPI/GFXContextBase.h"
#include "../Core/Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "Plane.h"
#include "../Core/Texture/Texture.h"
#include "../Core/Sampler.h"

namespace FraplesDev
{
	FraplesDev::Sheet::Sheet(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
		:
		BaseObject(gfx, rng, adist, ddist, odist, rdist)
	{
		
			struct Vertex
			{
				DirectX::XMFLOAT3 pos;

				struct
				{
					float u;
					float v;
				}tex;
			};
			auto model = Plane::Make<Vertex>();
			
			model._mVertices[0].tex = { 0.0f, 0.0f };
			model._mVertices[1].tex = { 1.0f, 0.0f };
			model._mVertices[2].tex = { 0.0f, 1.0f };
			model._mVertices[3].tex = { 1.0f, 1.0f };

			AddBind(std::make_shared<Texture>(gfx, Surface::FromFile("Images\\car.png")));

			AddBind(std::make_shared<VertexBuffer>(gfx, model._mVertices));
			AddBind(std::make_shared<Sampler>(gfx));

			auto pvs = std::make_shared<VertexShader>(gfx, L"TextureVS.cso");
			auto pvsbc = pvs->GetBytecode();
			AddBind(std::move(pvs));

			AddBind(std::make_shared<PixelShader>(gfx, L"TexturePS.cso"));

			const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
			{
				{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
				{"TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
			};
			AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));
			AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	
		AddBind(std::make_shared<TransformCBuf>(gfx, *this));
	}

	const DirectX::XMMATRIX FraplesDev::Sheet::GetTransformXM() const noexcept
	{
		return std::move(DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi));
	}
}