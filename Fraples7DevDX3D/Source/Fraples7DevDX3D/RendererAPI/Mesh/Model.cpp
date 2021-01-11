#include "Model.h"
#include "NodeSystem.h"
#include "ModelException.h"
#include "assimp/Importer.hpp"
#include  "assimp/postprocess.h"
#include "Core/Math/FraplesXM.h"
#include "Core/Math/Math.h"
namespace FraplesDev
{
	
	
	Model::Model(Graphics& gfx, const std::string& path, float scale)
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (pScene==nullptr)
		{
			throw ModelException(imp.GetErrorString(), __LINE__, __FILE__);
		}
		//parse materials
		std::vector<Material> materials;
		materials.reserve(pScene->mNumMaterials);
		for (size_t i = 0; i < pScene->mNumMaterials; i++)
		{
			materials.emplace_back(gfx, *pScene->mMaterials[i], path);
		}
		for (int i = 0; i < pScene->mNumMeshes; i++)
		{
			const auto& mesh = *pScene->mMeshes[i];
			_mMeshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, scale));
		}
		int nextId;
		_mRoot = ParseNode(nextId, *pScene->mRootNode, scale);
	}

	void Model::Submit() const noexcept(!IS_DEBUG)
	{
		_mRoot->Submit(DirectX::XMMatrixIdentity());
	}

	std::unique_ptr<Node>Model::ParseNode(int& nextId, const aiNode& node, float scale)noexcept
	{
		
		const auto transform = ScaleTranslation(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation))),scale);
		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(node.mNumMeshes);

		for (size_t i = 0; i < node.mNumMeshes; i++)
		{
			const auto meshIdx = node.mMeshes[i];
			curMeshPtrs.push_back(_mMeshPtrs.at(meshIdx).get());
		}
		auto pNode = std::make_unique<Node>(nextId++,node.mName.C_Str(), std::move(curMeshPtrs), transform);
		for (size_t i = 0; i < node.mNumChildren; i++)
		{
			pNode->AddChild(ParseNode(nextId, *node.mChildren[i],scale));
		}
		return pNode;
	}
	void Model::LinkTechniques(RenderGraph& rg)
	{
		for (auto& pMesh : _mMeshPtrs)
		{
			pMesh->LinkTechniques(rg);
		}
	}
	void Model::SetRootTransform(DirectX::FXMMATRIX tf)
	{
		_mRoot->SetAppliedTransform(tf);
	}
	void Model::Accept(ModelProbe& probe)
	{
		_mRoot->Accept(probe);
	}
	Model::~Model()
	{
	}
}