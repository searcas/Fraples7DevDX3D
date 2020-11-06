#include "Model.h"
#include <memory>
#include <sstream>
#include "RendererAPI/GFXContextBase.h"
#include "Core/Math/FraplesXM.h"
#include "Core/MetaProgramming/DynamicConstant.h"
#include "RendererAPI/ConstantBuffersEx.h"
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
	Model::Model(Graphics& gfx, const std::string& path, float scale) : _mpWindow(std::make_unique<ModelWindow>())
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

	void Model::Render(Graphics& gfx) const
	{
		if (auto node = _mpWindow->GetSelectedNode())
		{
			node->SetAppliedTransform(_mpWindow->GetTransform());
		}
		_mRoot->Render(gfx, DirectX::XMMatrixIdentity());
	}
	std::unique_ptr<Mesh>Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials,const std::filesystem::path& path,const float& scale)
	{
	
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
			if (material.GetTexture(aiTextureType_NORMALS,0,&texFileName) ==aiReturn_SUCCESS)
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
			auto pvsbyte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, hasAlphaDiffuse?"PhongPSSpecNormalMask.cso": "PhongPSSpecNormalMap.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));

			Node::PSMaterialConstantFullmonte pmc;
			pmc.specularPower = shininess;
			pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;
			//this is clearly an issue... all meshes will share same mat const, but may have different
			//Ns (Specular power) specified for each in the material properties ...bad conflict
			bindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));
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

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongNormalMapPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			auto layout = std::make_shared<MP::Struct>(0);
			layout->Add<MP::Float>("specularIntensity");
			layout->Add<MP::Float>("specularPower");
			layout->Add<MP::Bool>("normalMapEnabled");
			layout->Add<MP::Float>("padding");

			MP::Buffer cbuf{ std::move(layout) };
			cbuf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
			cbuf["specularPower"] = shininess;
			cbuf["normalMapEnabled"] = TRUE;
			bindablePtrs.push_back(std::make_shared<PixelConstantBufferEx>(gfx, cbuf, 1u));

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
			pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
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

			Node::PSMaterialConstantNotex pmc;
			pmc.specularPower = shininess;
			pmc.specularColor = specularColor;
			pmc.materialColor = diffuseColor;
			bindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
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
			auto pvsbyte = pvs->GetBytecode();

			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongSpecPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			struct PSMaterialConstant
			{
				float specularPowerConst = 20.0f;
				BOOL hasGloss = FALSE;
				float specularMapWeight = 1.0f;
			}pmc;
			pmc.specularPowerConst = shininess;
			pmc.hasGloss = hasAlphaGloss ? TRUE : FALSE;
			pmc.specularMapWeight = 1.0f;
			bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));


		}
		else
		{

			throw std::runtime_error("Terrible combination of textures in material, watch out what you doing or importing.");
	    }
		// anything with alpha diffuse is 2-sided IN THIS SCENE
		//need a better way of signalling  2-sidedness to be more general in the future
		bindablePtrs.push_back(Rasterizer::Resolve(gfx, hasAlphaDiffuse));
		bindablePtrs.push_back(Blending::Resolve(gfx, false));
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

	void Model::ShowModelInfo(Graphics& gfx, const char* windowName)
	{
		_mpWindow->Show(gfx, windowName, *_mRoot);
	}

	void Model::ModelWindow::Show(Graphics& gfx,const char* windowName, const Node& root)
	{
		windowName = windowName ? windowName : "Model ";

		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);

			root.RenderTree(_mPselectedNode);

			ImGui::NextColumn();
			if (_mPselectedNode != nullptr)
			{
				const auto id = _mPselectedNode->GetId();
				auto i = _mTransforms.find(id);
				if (i ==_mTransforms.end())
				{
					const auto& applied = _mPselectedNode->GetAppliedTransform();
					const auto angles = ExtractEulerAngles(applied);
					const auto translation = ExtractTranslation(applied);
					TransformParameters tp;
					tp.roll = angles.z;
					tp.pitch = angles.x;
					tp.yaw = angles.y;

					tp.x = translation.x;
					tp.y = translation.y;
					tp.z = translation.z;

					std::tie(i, std::ignore) = _mTransforms.insert({ id,tp });
				}
				auto& transform = i->second;
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

				if (!_mPselectedNode->ControlMeSenpai(gfx,_mSkinMaterial))
				{
					_mPselectedNode->ControlMeSenpai(gfx, _mRingMaterial);
				}
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