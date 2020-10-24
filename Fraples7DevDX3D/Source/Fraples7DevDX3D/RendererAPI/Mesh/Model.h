#pragma once
#include "RendererAPI/GFXContextBase.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "RendererAPI/RendererBase.h"
#include "GraphicAPI/Graphics.h"

namespace FraplesDev
{
	class Mesh : public RendererBase<Mesh>
	{
	public:
		Mesh(Graphics& gfx,std::vector<std::unique_ptr<GfxContext>>bindPtrs)
		{
			if (!IsStaticInitialized())
			{
				AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			}	
			for (auto& pb : bindPtrs)	
			{	
				if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))	
				{	
					AddIndexBuffer(std::unique_ptr<IndexBuffer>{pi});	
					pb.release();	
				}	
				else	
				{	
					AddBind(std::move(pb));	
				}	
			}	
			AddBind(std::make_unique<TransformCBuf>(gfx, *this));	
			
		}
		void Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
		{
			DirectX::XMStoreFloat4x4(&_mTransform, accumulatedTransform);
			Renderer::Render(gfx);
		}
		const DirectX::XMMATRIX GetTransformXM()const noexcept override
		{
			return DirectX::XMLoadFloat4x4(&_mTransform);
		}

	private:
		mutable DirectX::XMFLOAT4X4 _mTransform;
	};

	class Node
	{
		friend class Model;
	public:
		Node(std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG)
			:_mMeshPtrs(std::move(meshPtrs))
		{
			DirectX::XMStoreFloat4x4(&this->_mTransform, transform);
		}
		void Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
		{
			const auto built = DirectX::XMLoadFloat4x4(&_mTransform) * accumulatedTransform;

			for (const auto pm: _mMeshPtrs)
			{
				pm->Render(gfx, built);
			}
			for (const auto& pc : _mChildPtrs)
			{
				pc->Render(gfx, built);
			}
		}
	private:
		void AddChild(std::unique_ptr<Node>pChild)noexcept(!IS_DEBUG)
		{
			assert(pChild);
			_mChildPtrs.push_back(std::move(pChild));

		}
	private:
		std::vector<std::unique_ptr<Node>>_mChildPtrs;
		std::vector<Mesh*>_mMeshPtrs;
		DirectX::XMFLOAT4X4 _mTransform;
	};

	class Model
	{
	public:
		Model(Graphics& gfx, const std::string fileName)
		{
			Assimp::Importer imp;
			const auto pScene = imp.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
			for (int i = 0; i < pScene->mNumMeshes; i++)
			{
				_mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
			}
			_mRoot = ParseNode(*pScene->mRootNode);
		}
		static std::unique_ptr<Mesh>ParseMesh(Graphics& gfx, const aiMesh& mesh)
		{
			using MP::VertexLayout;
			MP::VertexBuffer vBuf(std::move(VertexLayout{}.Append(VertexLayout::Position3D).Append(VertexLayout::Normal)));
			
			for (unsigned int i = 0;i <mesh.mNumVertices; i++)
			{
				vBuf.EmplaceBack(*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]), 
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]));

			}
			std::vector<unsigned short> indices;
			indices.reserve(mesh.mNumFaces * 3);
			for (unsigned int i = 0; i < mesh.mNumFaces; i++)
			{
				const auto& face = mesh.mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			std::vector<std::unique_ptr<GfxContext>>bindablePtrs;
			bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vBuf));
			bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

			auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
			auto pvsByte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));
			bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
			bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vBuf.GetLayout().GetD3DLayout(),pvsByte));

			struct PSMaterialConstant
			{
				DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
				float specularIntensity = 0.6f;
				float specularPower = 30.0f;
				float padding[3];
			}pixelShaderMaterialConstant;
			bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pixelShaderMaterialConstant, 1u));
			return std::make_unique<Mesh>(gfx,std::move( bindablePtrs ));
		}
		std::unique_ptr<Node>ParseNode(const aiNode& node)
		{
			const auto transform = DirectX::XMMatrixTranspose
			(DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)));
			std::vector<Mesh*> curMeshPtrs;
			curMeshPtrs.reserve(node.mNumMeshes);

			for (size_t i = 0; i < node.mNumMeshes; i++)
			{
				const auto meshIdx = node.mMeshes[i];
				curMeshPtrs.push_back(_mMeshPtrs.at(meshIdx).get());
			}
			auto pNode = std::make_unique<Node>(std::move(curMeshPtrs), transform);
			for (size_t i = 0; i < node.mNumChildren; i++)
			{
				pNode->AddChild(ParseNode(*node.mChildren[i]));
			}
			return pNode;
		}
		void Render(Graphics& gfx)const
		{
			_mRoot->Render(gfx, DirectX::XMMatrixIdentity());
		}
	private:
		std::unique_ptr<Node>_mRoot;
		std::vector<std::unique_ptr<Mesh>>_mMeshPtrs;

	};

}