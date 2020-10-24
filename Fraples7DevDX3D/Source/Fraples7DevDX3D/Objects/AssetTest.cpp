#include "AssetTest.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "RendererAPI/GFXContextBase.h"
#include "../Core/MetaProgramming/Vertex.h"
namespace FraplesDev
{
	Asset::Asset(Graphics& gfx, 
		std::mt19937& rng, 
		std::uniform_real_distribution<float>& adist, 
		std::uniform_real_distribution<float>& ddist, 
		std::uniform_real_distribution<float>& odist, 
		std::uniform_real_distribution<float>& rdist, 
		DirectX::XMFLOAT3 material, float scale) : BaseObject(gfx,rng,adist,ddist,odist,rdist)
	{
		if (!IsStaticInitialized())
		{
			MP::VertexBuffer vbuf(std::move(MP::VertexLayout{}.
				Append(MP::VertexLayout::Position3D).
				Append(MP::VertexLayout::Normal) ));

			Assimp::Importer imp;
			auto pModel = imp.ReadFile("models\\FinalBaseMesh.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
			const auto pMesh = pModel->mMeshes[0];
			
			for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
			{
				vbuf.EmplaceBack(
					DirectX::XMFLOAT3{pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale,pMesh->mVertices[i].z * scale },
					*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]));
			}
			std::vector<unsigned short> indices;
			indices.reserve(pMesh->mNumFaces * 3);
			for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
			{
				const auto& face = pMesh->mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
			AddStaticBind(std::make_unique<VertexBuffer>(gfx, vbuf));

			AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
			auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
			auto pvsbyte = pvs->GetBytecode();

			AddStaticBind(std::move(pvs));

			AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

			AddStaticBind(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbyte));
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			
			struct PSMaterialConstant
			{
				DirectX::XMFLOAT3 color;
				float specularIntensity = 0.6f;
				float specularPower = 0.6f;
				float padding[3];
			}pixelShaderMaterialConstant;
			pixelShaderMaterialConstant.color = material;
			AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pixelShaderMaterialConstant, 1u));
		}
		else
		{
			SetIndexFromStatic();
		}
		AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	}
}