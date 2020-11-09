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
	const MP::Buffer* Node::GetMaterialConstants()const noexcept(!IS_DEBUG)
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
			if (auto mat = _mpWindow->GetMaterial())
			{
				node->SetMaterialConstants(*mat);
			}
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
			auto pvsbyte = pvs->GetBytecode();
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

			auto buf = MP::Buffer::Make(std::move(layout));
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
			auto pvsbyte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongNormalMapPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			MP::RawLayout layout;
			layout.Add<MP::Float>("specularIntensity");
			layout.Add<MP::Float>("specularPower");
			layout.Add<MP::Bool>("normalMapEnabled");
			MP::Buffer cbuf = MP::Buffer::Make(std::move(layout));
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
			auto pvsbyte = pvs->GetBytecode();
			bindablePtrs.push_back(std::move(pvs));
			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			MP::RawLayout layout;
			layout.Add<MP::Float4>("materialColor");
			layout.Add<MP::Float4>("specularColor");
			layout.Add<MP::Float>("specularPower");
			auto buf = MP::Buffer::Make(std::move(layout));
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
			auto pvsbyte = pvs->GetBytecode();

			bindablePtrs.push_back(std::move(pvs));

			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongSpecPS.cso"));
			bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));
			
			MP::RawLayout layout;
			layout.Add<MP::Float>("specularPowerConst");
			layout.Add<MP::Bool>("hasGloss");
			layout.Add<MP::Float>("specularMapWeight");

			auto buf = MP::Buffer::Make(std::move(layout));
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
		auto pvsbyte = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbyte));

		MP::RawLayout layout;
		layout.Add<MP::Float>("specularIntensity");
		layout.Add<MP::Float>("specularPower");
		auto buf = MP::Buffer::Make(std::move(layout));
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

					auto pMatConst = _mPselectedNode->GetMaterialConstants();
					auto buf = pMatConst != nullptr ? std::optional<MP::Buffer>{*pMatConst} : std::optional<MP::Buffer>{};
					std::tie(i, std::ignore) = _mTransforms.insert({ id,{tp,std::move(buf)} });
				}
				// Link imgui ctrl to our cached transform params
				auto& transform = i->second.transformParams;
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

				// link imgui ctrl to our cached material params
				if (i->second.materialCbuf)
				{
					auto& mat = *i->second.materialCbuf;
					ImGui::Text("Material");
					if (auto v = mat["normalMapEnabled"]; v.Exists())
					{
						ImGui::Checkbox("Norm Map", &v);
					}
					if (auto v = mat["specularMapEnabled"]; v.Exists())
					{
						ImGui::Checkbox("Spec Map", &v);
					}
					if (auto v = mat["hasGlossMap"]; v.Exists())
					{
						ImGui::Checkbox("Gloss Map", &v);
					}
					if (auto v = mat["materialColor"]; v.Exists())
					{
						ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v)));
					}
					if (auto v = mat["specularPower"]; v.Exists())
					{
						ImGui::SliderFloat("Spec Power", &v, 0.0f, 100.0f, "%.1f");
					}
					if (auto v = mat["specularColor"]; v.Exists())
					{
						ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v)));
					}
					if (auto v = mat["specularMapWeight"]; v.Exists())
					{
						ImGui::SliderFloat("Spec Weight", &v, 0.0f, 4.0f);
					}
					if (auto v = mat["specularIntensity"]; v.Exists())
					{
						ImGui::SliderFloat("Spec Intens", &v, 0.0f, 1.0f);
					}
				}
			}
		}
		ImGui::End();
	}

	DirectX::XMMATRIX Model::ModelWindow::GetTransform() const noexcept
	{
		assert(_mPselectedNode != nullptr);

		const auto& transform = _mTransforms.at(_mPselectedNode->GetId()).transformParams;
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	const MP::Buffer* Model::ModelWindow::GetMaterial()const noexcept
	{
		assert(_mPselectedNode != nullptr);
		const auto& mat = _mTransforms.at(_mPselectedNode->GetId()).materialCbuf;
		return mat ? &*mat : nullptr;
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