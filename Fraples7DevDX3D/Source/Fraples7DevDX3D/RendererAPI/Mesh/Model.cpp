#include "Model.h"
#include <memory>
#include <sstream>
#include "RendererAPI/GFXContextBase.h"
namespace FraplesDev
{
	
	Node::Node(int id, const std::string&name,std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG)
		:_mName(name),_mMeshPtrs(std::move(meshPtrs)),_mID(id)
	{
		DirectX::XMStoreFloat4x4(&_mTransform, transform);
		DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
	}
	void Node::Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG)
	{
		
		const auto built = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&_mTransform) * accumulatedTransform;

		for (const auto pm : _mMeshPtrs)
		{
			pm->Render(gfx, built);
		}
		for (const auto& pc : _mChildPtrs)
		{
			pc->Render(gfx, built);
		}
	}

	void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
	{
		DirectX::XMStoreFloat4x4(&appliedTransform, transform);
	}

	void Node::RenderTree(Node*& pSelectedNode) const noexcept
	{
		//if there is no selected node, set selectedId to an impossible value
		const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
		//noindex serves as the uid for gui tree nodes, incremented troughout recursion
		const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow | 
			((GetId() == selectedId) ? 
				ImGuiTreeNodeFlags_Selected : 0) | 
				((_mChildPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

		//if tree node expanded, recurively render all children
		auto ifClicked = ImGui::TreeNodeEx((void*)GetId(), node_flags, _mName.c_str());
		
		
			if (ImGui::IsItemClicked())
			{
				pSelectedNode = const_cast<Node*>(this);
			}
			if (ifClicked)
			{
				for (const auto& pChild : _mChildPtrs)
				{
					pChild->RenderTree(pSelectedNode);
				}
				ImGui::TreePop();
			}
	}

	void Node::AddChild(std::unique_ptr<Node>pChild)noexcept(!IS_DEBUG)
	{
		assert(pChild);
		_mChildPtrs.push_back(std::move(pChild));

	}
	Model::Model(Graphics& gfx, const std::string fileName) : _mpWindow(std::make_unique<ModelWindow>())
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (pScene==nullptr)
		{
			throw ModelException(imp.GetErrorString(), __LINE__, __FILE__);
		}
		for (int i = 0; i < pScene->mNumMeshes; i++)
		{
			_mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i],pScene->mMaterials));
		}
		int nextId;
		_mRoot = ParseNode(nextId,*pScene->mRootNode);
	}

	void Model::Render(Graphics& gfx) const
	{
		if (auto node = _mpWindow->GetSelectedNode())
		{
			node->SetAppliedTransform(_mpWindow->GetTransform());
		}
		_mRoot->Render(gfx, DirectX::XMMatrixIdentity());
	}
	std::unique_ptr<Mesh>Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
	{
	
		using namespace std::string_literals;

		std::vector<std::shared_ptr<GfxContext>>bindablePtrs;

		const auto base = "Models\\gobber\\"s;
		bool hasSpecularMap = false;
		bool hasNormalMap = false;
		bool hasDiffuseMap = false;
		float shininess = 35.0f;

		if (mesh.mMaterialIndex >=0)
		{
			auto& material = *pMaterials[mesh.mMaterialIndex];

		
			aiString texFileName;

			if (material.GetTexture(aiTextureType_DIFFUSE,0,&texFileName) ==aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(Texture::Resolve(gfx, base + texFileName.C_Str()));
				hasDiffuseMap = true;
			}
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(Texture::Resolve(gfx, base + texFileName.C_Str(),1));
				hasSpecularMap = true;
			}
			else 
			{
				material.Get(AI_MATKEY_SHININESS, shininess);
			}
			if (material.GetTexture(aiTextureType_NORMALS,0,&texFileName) ==aiReturn_SUCCESS)
			{
				bindablePtrs.push_back(Texture::Resolve(gfx, base + texFileName.C_Str(), 2));
				hasNormalMap = true;
			}
			if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
			{
				bindablePtrs.push_back(Sampler::Resolve(gfx));
			}
		}
		const auto meshTag = base + "%" + mesh.mName.C_Str();
		const float scale = 6.0f;
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
			auto pvsbyte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpecNormalMap.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));

			struct PSMaterialConstantFullmonte
			{
				BOOL normalMapEnabled = TRUE;
				float padding[3];
			}pmc;
			//this is clearly an issue... all meshes will share same mat const, but may have different
			//Ns (Specular power) specified for each in the material properties ...bad conflict
			bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));
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
			auto pvsbyte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx,"PhongNormalMapPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			struct PSMaterialConstantDiffnorm
			{
				float specularIntensity = 0.18;
				float specularPower = 0.0f;
				BOOL normalMapEnabled = TRUE;
				float padding[1] = {};
			}pmc;
			pmc.specularPower = shininess;
			bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve(gfx, pmc, 1u));
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
			auto pvsbyte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));

			struct PSMaterialConstantDiffnorm
			{
				float specularIntensity = 0.18;
				float specularPower = 0.0f;
				float padding[2] = {};
			}pmc;
			pmc.specularPower = shininess;
			bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve(gfx, pmc, 1u));

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
			auto pvsbyte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));
			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));

			struct PSMaterialConstantNotex
			{
				DirectX::XMFLOAT4 materialColor = { 0.65f,0.65f,0.85f,1.0f };
				float specularIntensity = 0.18f;
				float specularPower = 0.0f;
				float padding[2] = { };
			} pmc;
			pmc.specularPower = shininess;

			bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
		}
		else
		{

			throw std::runtime_error("Terrible combination of textures in material, watch out what you doing or importing.");
	    }
		

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
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

	void Model::ShowModelInfo(const char* windowName)
	{
		_mpWindow->Show(windowName, *_mRoot);
	}

	void Model::ModelWindow::Show(const char* windowName, const Node& root)
	{
		windowName = windowName ? windowName : "Model";

		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);

			root.RenderTree(_mPselectedNode);

			ImGui::NextColumn();
			if (_mPselectedNode != nullptr)
			{
				auto& transform = _mTransforms[_mPselectedNode->GetId()];

				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
				if (ImGui::Button("Reset"))
				{
					transform = { };
				}
			}

		}
		ImGui::End();
	}

	DirectX::XMMATRIX Model::ModelWindow::GetTransform() const noexcept
	{
		assert(_mPselectedNode != nullptr);

		const auto& transform = _mTransforms.at(_mPselectedNode->GetId());
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	Node* Model::ModelWindow::GetSelectedNode()	const noexcept
	{
		return _mPselectedNode;
	}
	Model::ModelException::ModelException(std::string note, int line, const char* file) 
		: FraplesException(line,file), note(std::move(note))
	{

	}
	const char* Model::ModelException::what() const noexcept
	{
		std::ostringstream oss;
		oss << FraplesException::what() << std::endl << "[Note] " << GetNote();
		_mWhatBuffer = oss.str();
		return _mWhatBuffer.c_str();
	}
	const char* Model::ModelException::GetType() const noexcept
	{
		return "Fraples7 Studio Engine Exception";
	}
	const std::string& Model::ModelException::GetNote() const noexcept
	{
		return note;
	}
}