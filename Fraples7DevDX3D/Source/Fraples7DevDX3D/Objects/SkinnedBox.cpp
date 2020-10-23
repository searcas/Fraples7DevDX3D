#include "SkinnedBox.h"
#include "Cube.h"
#include "../RendererAPI/GFXContextBase.h"
#include "../Core/Texture/Texture.h"	
namespace FraplesDev
{
	SkinnedBox::SkinnedBox(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
		:BaseObject(gfx,rng,adist,ddist,odist,rdist)
	{
		if (!IsStaticInitialized())
		{
			struct Vertex
			{
				DirectX::XMFLOAT3 pos;
				DirectX::XMFLOAT3 normal;
				DirectX::XMFLOAT2 tc;
			};

			auto model = Cube::MakeIndependentTextured<Vertex>();
			model.SetNormalsIndependentFlat();
	
			AddStaticBind(std::make_unique<VertexBuffer>(gfx, model._mVertices));

			AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\kappa50.png")));

			AddStaticBind(std::make_unique<Sampler>(gfx));
			auto pvs = std::make_unique<VertexShader>(gfx, L"TexturePhongVS.cso");
			auto pvsbyte = pvs->GetBytecode();
			AddStaticBind(std::move(pvs));

			AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePhongPS.cso"));
			AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model._mIndices));

			const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
			{
				{"Position", 0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
				{"Normal", 0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
				{"TexCoord", 0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
			};
			AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbyte));
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			struct PSMaterialConstant
			{
				float specularIntensity = 0.6f;
				float specularPower = 30.0f;
				float padding[2];
			}colorConst;
			AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));
		}
		else
		{
			SetIndexFromStatic();
		}

		AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	}

}