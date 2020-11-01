#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "GraphicAPI/Graphics.h"
#include "Core/Math/Math.h"
#include "ImGui/imgui.h"
#include "Mesh.h"
#include "Core/Debugging/Exceptions/FraplesException.h"
#include <unordered_map>
#include <optional>
#include "RendererAPI/ConstantBuffers.h"
#include <filesystem>
namespace FraplesDev
{
	

	class Node
	{
	
		friend class Model;
	public:
		struct PSMaterialConstantFullmonte
		{
			BOOL normalMapEnabled = TRUE;
			BOOL specularMapEnabled = TRUE;
			BOOL hasGlossMap = FALSE;
			float specularPower = 3.1f;
			DirectX::XMFLOAT3 specularColor = { 0.75f, 0.75f, 0.75f };
			float specularMapWeight = 0.671f;
		};
		struct PSMaterialConstantNotex
		{
			DirectX::XMFLOAT4 materialColor = { 0.447970f, 0.327254f, 0.176283f, 1.0f };
			DirectX::XMFLOAT4 specularColor = { 0.65f, 0.65f, 0.65f, 1.0f };
			float specularPower = 120.0f;
			float padding[3] = {};
		};
	public:
		Node(int id,const std::string& name,std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG);
		void Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		void SetAppliedTransform(DirectX::FXMMATRIX transform)noexcept;
		inline const DirectX::XMFLOAT4X4& GetAppliedTransform() noexcept { return appliedTransform; }
		inline const int GetId()const noexcept { return _mID; }
	private:
		void AddChild(std::unique_ptr<Node>pChild)noexcept(!IS_DEBUG);
		void RenderTree(Node*& pSelectedNode) const noexcept;

		template<class T>
		bool ControlMeSenpai(Graphics& gfx, T& c)
		{

			if (_mMeshPtrs.empty())
			{
				return false;
			}

			if constexpr(std::is_same<T, PSMaterialConstantFullmonte>::value)
			{
				if (auto pcb = _mMeshPtrs.front()->QueryBindable<PixelConstantBuffer<T>>())
				{
					ImGui::Text("Material");

					bool normalMapEnabled = (bool)c.normalMapEnabled;
					ImGui::Checkbox("Norm Map", &normalMapEnabled);
					c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

					bool specularMapEnabled = (bool)c.specularMapEnabled;
					ImGui::Checkbox("Spec Map", &specularMapEnabled);
					c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

					bool hasGlossMap = (bool)c.hasGlossMap;
					ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
					c.hasGlossMap = hasGlossMap ? TRUE : FALSE;

					ImGui::SliderFloat("Spec Weight", &c.specularMapWeight, 0.0f, 2.0f);

					ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f");

					ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));

					pcb->Update(gfx, c);
					return true;
				}
			}
			else if constexpr (std::is_same<T, PSMaterialConstantNotex>::value)
			{
				if (auto pcb = _mMeshPtrs.front()->QueryBindable<PixelConstantBuffer<T>>())
				{
					ImGui::Text("Material");

					ImGui::ColorPicker3("Spec Color.", reinterpret_cast<float*>(&c.specularColor));

					ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f");

					ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&c.materialColor));

					pcb->Update(gfx, c);
					return true;
				}
			}
			return false;
		}
	private:
		int _mID = 0;
		std::string _mName = {};
		std::vector<std::unique_ptr<Node>>_mChildPtrs = {};
		std::vector<Mesh*>_mMeshPtrs = {};
		DirectX::XMFLOAT4X4 _mTransform = {};
		DirectX::XMFLOAT4X4 appliedTransform = {};

	};

	class Model
	{
	public:
		class ModelException : public FraplesException
		{
		public:
			ModelException(std::string note, int line, const char* file);
			const char* what() const noexcept override;
			const char* GetType()const noexcept override;
			const std::string& GetNote()const noexcept;
		private:
			std::string note = "";
		};
		class ModelWindow
		{
		public:
			void Show(Graphics& gfx, const char* windowName, const Node& root);

			DirectX::XMMATRIX GetTransform() const noexcept;
			Node* GetSelectedNode()	const noexcept;
		private:
			Node* _mPselectedNode = nullptr;
			struct TransformParameters
			{
				float roll = 0.0f;
				float pitch = 0.0f ;
				float yaw = 0.0f;
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
			};
			Node::PSMaterialConstantFullmonte _mSkinMaterial = {};
			Node::PSMaterialConstantNotex _mRingMaterial = {};
			std::unordered_map<int, TransformParameters>_mTransforms = {};
		};


		Model(Graphics& gfx, const std::string& path, float scale = 1.0f);
		void Render(Graphics& gfx) const;
		static std::unique_ptr<Mesh>ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path,const float& scale);
		std::unique_ptr<Node>ParseNode(int& nextId,const aiNode& node)noexcept;
		void SetRootTransform(DirectX::FXMMATRIX tf);
		~Model();
		void ShowModelInfo(Graphics& gfx, const char* windowName);
	private:
		std::unique_ptr<Node>_mRoot;
		std::vector<std::unique_ptr<Mesh>>_mMeshPtrs;
		std::unique_ptr<class ModelWindow>_mpWindow;

	};


}