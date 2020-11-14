#include "Model.h"
#include "NodeSystem.h"
#include "ModelException.h"
#include "assimp/Importer.hpp"
#include  "assimp/postprocess.h"
namespace FraplesDev
{
	
	
	/*const MP::Buffer* Node::GetMaterialConstants()const noexcept(!IS_DEBUG)
	{
		if (_mMeshPtrs.size() == 0)
		{
			return nullptr;
		}
		auto pBindable = _mMeshPtrs.front()->QueryBindable<CachingPixelConstantBufferEx>();
		return &pBindable->GetBuffer();
	}
	void Node::SetMaterialConstants(const MP::Buffer& buf_in)noexcept(!IS_DEBUG)
	{
		auto pcb = _mMeshPtrs.front()->QueryBindable<CachingPixelConstantBufferEx>();
		assert(pcb != nullptr);
		pcb->SetBuffer(buf_in);
	}*/
	
	Model::Model(Graphics& gfx, const std::string& path, float scale)
		: _mpWindow(std::make_unique<ModelWindow>())
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (pScene==nullptr)
		{
			throw ModelException(imp.GetErrorString(), __LINE__, __FILE__);
		}
		for (int i = 0; i < pScene->mNumMeshes; i++)
		{
			_mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i],pScene->mMaterials,path,scale));
		}
		int nextId;
		_mRoot = ParseNode(nextId,*pScene->mRootNode);
	}

	void Model::Submit(FrameCommander& frame) const noexcept(!IS_DEBUG)
	{
		// I'm still not happy about updating parameters
		// (I.E mutatting a bindable GPU state which is 
		// part of a node which is part of the model that
		// is const in this call) can probably do this elsewhere
		_mpWindow->ApplyParameters();
		_mRoot->Submit(frame, DirectX::XMMatrixIdentity());
	}
	std::unique_ptr<Mesh>Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials,const std::filesystem::path& path,const float& scale)
	{
	/*
		using namespace std::string_literals;
			
		const auto rootPath = path.parent_path().string() + "\\";
		std::vector<std::shared_ptr<GfxContext>>bindablePtrs;

		bool hasSpecularMap = false;
		bool hasNormalMap = false;
		bool hasAlphaDiffuse = false;
		bool hasAlphaGloss = false;
		bool hasDiffuseMap = false;
		float shininess = 2.0f;
		DirectX::XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };
		DirectX::XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };

		if (mesh.mMaterialIndex >=0)
		{
			auto& material = *pMaterials[mesh.mMaterialIndex];

		
			aiString texFileName;

			if (material.GetTexture(aiTextureType_DIFFUSE,0,&texFileName) == aiReturn_SUCCESS)
			{
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				hasAlphaDiffuse = tex->HasAlhpa();
				bindablePtrs.push_back(std::move(tex));
				hasDiffuseMap = true;
			}
			else
			{
				material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
			}
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
				hasAlphaGloss = tex->HasAlhpa();
				bindablePtrs.push_back(std::move(tex));
				hasSpecularMap = true;
			}
			else
			{
				material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(specularColor));
			}
			if(!hasAlphaGloss)
			{
				material.Get(AI_MATKEY_SHININESS, shininess);
			}
			if (material.GetTexture(aiTextureType_NORMALS,0,&texFileName) == aiReturn_SUCCESS)
			{
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2);
				hasAlphaGloss = tex->HasAlhpa();
				bindablePtrs.push_back(std::move(tex));
				hasNormalMap = true;
			}
			if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
			{
				bindablePtrs.push_back(Sampler::Resolve(gfx));
			}
		}
		const auto meshTag = path.string() + "%" + mesh.mName.C_Str();
		if (hasDiffuseMap && hasNormalMap && hasSpecularMap)
		{
			MP::VertexBuffer vbuf(std::move(MP::VertexLayout{}.Append(MP::ElementType::Position3D).Append(MP::ElementType::Normal).Append(MP::ElementType::Tangent).Append(MP::ElementType::Bitangent).Append(MP::ElementType::Texture2D)));

			for (unsigned int i = 0; i < mesh.mNumVertices; i++)
			{
				vbuf.EmplaceBack(DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
					*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
			}
			std::vector<unsigned short>indices;
			indices.reserve(mesh.mNumFaces * 3);
			for (unsigned int i = 0; i < mesh.mNumFaces; i++)
			{
				const auto& face = mesh.mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
			bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
			bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

			auto pvs = VertexShader::Resolve(gfx, "PhongNormalMapVS.cso");
			auto pvsbyte = pvs->GetByteCode();
			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, hasAlphaDiffuse?"PhongPSSpecNormalMask.cso": "PhongPSSpecNormalMap.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			MP::RawLayout layout;
			layout.Add<MP::Bool>("normalMapEnabled");
			layout.Add<MP::Bool>("specularMapEnabled");
			layout.Add<MP::Bool>("hasGlossMap");
			layout.Add<MP::Float>("specularPower");
			layout.Add<MP::Float3>("specularColor");
			layout.Add<MP::Float>("specularMapWeight");

			auto buf = MP::Buffer(std::move(layout));
			buf["normalMapEnabled"] = true;
			buf["specularMapEnabled"] = true;
			buf["hasGlossMap"] = hasAlphaGloss;
			buf["specularPower"] = shininess;
			buf["specularColor"] = DirectX::XMFLOAT3{ 0.75f,0.75f,0.75f };
			buf["specularMapWeight"] = 0.671f;
			bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
		}
		else if (hasDiffuseMap && hasNormalMap)
		{
			MP::VertexBuffer vbuf(std::move(MP::VertexLayout{}.
				Append(MP::ElementType::Position3D).
				Append(MP::ElementType::Normal).
				Append(MP::ElementType::Tangent).
				Append(MP::ElementType::Bitangent).
				Append(MP::ElementType::Texture2D)));



			for (unsigned int i = 0; i < mesh.mNumVertices; i++)
			{
				vbuf.EmplaceBack(
					DirectX::XMFLOAT3(
						mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
					*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
			}

			std::vector<unsigned short>indices;
			indices.reserve(mesh.mNumFaces * 3);
			for (unsigned int i = 0; i < mesh.mNumFaces; i++)
			{
				const auto& face = mesh.mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
			bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
			bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

			auto pvs = VertexShader::Resolve(gfx, "PhongNormalMapVS.cso");
			auto pvsbyte = pvs->GetByteCode();
			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongNormalMapPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			MP::RawLayout layout;
			layout.Add<MP::Float>("specularIntensity");
			layout.Add<MP::Float>("specularPower");
			layout.Add<MP::Bool>("normalMapEnabled");
			MP::Buffer cbuf = MP::Buffer(std::move(layout));
			cbuf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
			cbuf["specularPower"] = shininess;
			cbuf["normalMapEnabled"] = true;
			bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEx>(gfx, cbuf, 1u));
	
		}
		else if(!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
		{
			MP::VertexBuffer vbuf(MP::VertexLayout{}.Append(MP::ElementType::Position3D).
				Append(MP::ElementType::Normal));

		
			for (unsigned i = 0; i < mesh.mNumVertices; i++)
			{
				vbuf.EmplaceBack(
					DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
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
			bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
			bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

			auto pvs = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
			auto pvsbyte = pvs->GetByteCode();
			bindablePtrs.push_back(std::move(pvs));
			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			MP::RawLayout layout;
			layout.Add<MP::Float4>("materialColor");
			layout.Add<MP::Float4>("specularColor");
			layout.Add<MP::Float>("specularPower");
			auto buf = MP::Buffer(std::move(layout));
			buf["specularPower"] = shininess;
			buf["materialColor"] = diffuseColor;
			buf["specularColor"] = specularColor;
			bindablePtrs.push_back(std::make_unique<CachingPixelConstantBufferEx>(gfx, buf, 1u));
		}
		else if (hasDiffuseMap && !hasNormalMap && hasSpecularMap)
		{
			MP::VertexBuffer vbuf(std::move(MP::VertexLayout{}.Append(MP::ElementType::Position3D).Append(MP::ElementType::Normal).Append(MP::ElementType::Texture2D)));

			for (unsigned int i = 0; i< mesh.mNumVertices; i++)
			{
				vbuf.EmplaceBack(DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
			}
			std::vector<unsigned short>indices;
			indices.reserve(mesh.mNumFaces * 3);
			for (unsigned int i = 0; i < mesh.mNumFaces; i++)
			{
				const auto& face = mesh.mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
			bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
			bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

			auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
			auto pvsbyte = pvs->GetByteCode();

			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongSpecPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			MP::RawLayout layout;
			layout.Add<MP::Float>("specularPowerConst");
			layout.Add<MP::Bool>("hasGloss");
			layout.Add<MP::Float>("specularMapWeight");

			auto buf = MP::Buffer(std::move(layout));
			buf["specularPowerConst"] = shininess;
			buf["hasGloss"] = hasAlphaGloss;
			buf["specularMapWight"] = 1.0f;

			bindablePtrs.push_back(std::make_unique<CachingPixelConstantBufferEx>(gfx, buf, 1u));


		}
		else if (hasDiffuseMap)
		{
		MP::VertexBuffer vbuf(std::move(MP::VertexLayout{}.
			Append(MP::ElementType::Position3D).
			Append(MP::ElementType::Normal).
			Append(MP::ElementType::Texture2D)));



		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(
					mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
		}

		std::vector<unsigned short>indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbyte = pvs->GetByteCode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));

		MP::RawLayout layout;
		layout.Add<MP::Float>("specularIntensity");
		layout.Add<MP::Float>("specularPower");
		auto buf = MP::Buffer(std::move(layout));
		buf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		buf["specularPower"] = shininess;
		buf["specularMapWeight"] = 1.0f;

		bindablePtrs.push_back(std::make_unique<CachingPixelConstantBufferEx>(gfx, buf, 1u));
		}
		else
		{

			throw std::runtime_error("Terrible combination of textures in material, watch out what you doing or importing.");
	    }
		// anything with alpha diffuse is 2-sided IN THIS SCENE
		//need a better way of signalling  2-sidedness to be more general in the future
		bindablePtrs.push_back(Rasterizer::Resolve(gfx, hasAlphaDiffuse));
		bindablePtrs.push_back(Blending::Resolve(gfx, false));
		bindablePtrs.push_back(std::make_shared<Stencil>(gfx, Stencil::Mode::Off));
		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
		*/
		return {};
	}
	std::unique_ptr<Node>Model::ParseNode(int& nextId, const aiNode& node)noexcept
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
		auto pNode = std::make_unique<Node>(nextId++,node.mName.C_Str(), std::move(curMeshPtrs), transform);
		for (size_t i = 0; i < node.mNumChildren; i++)
		{
			pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
		}
		return pNode;
	}
	void Model::SetRootTransform(DirectX::FXMMATRIX tf)
	{
		_mRoot->SetAppliedTransform(tf);
	}
	Model::~Model()
	{
	}

	void Model::ShowModelInfo(Graphics& gfx, const char* windowName)
	{
		_mpWindow->Show(gfx, windowName, *_mRoot);
	}

	
	
}