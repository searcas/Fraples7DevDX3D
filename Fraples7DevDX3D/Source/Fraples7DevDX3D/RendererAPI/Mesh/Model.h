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
#include "Core/MetaProgramming/DynamicConstant.h"
namespace FraplesDev
{
	

	class Node
	{
	friend class Model;
	public:
		Node(int id,const std::string& name,std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG);
		void Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		void SetAppliedTransform(DirectX::FXMMATRIX transform)noexcept;
		inline const DirectX::XMFLOAT4X4& GetAppliedTransform() noexcept { return appliedTransform; }
		inline const int GetId()const noexcept { return _mID; }
		const MP::Buffer* GetMaterialConstants()const noexcept(!IS_DEBUG);
		void SetMaterialConstants(const MP::Buffer& buff)noexcept(!IS_DEBUG);
	private:
		void AddChild(std::unique_ptr<Node>pChild)noexcept(!IS_DEBUG);
		void RenderTree(Node*& pSelectedNode) const noexcept;
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
			const MP::Buffer& GetMaterial()const noexcept(!IS_DEBUG);
			

			bool TransformDirty()const noexcept(!IS_DEBUG);
			void ResetTransformDirty()noexcept(!IS_DEBUG);
			bool MaterialDirty()const noexcept(!IS_DEBUG);
			void ResetMaterialDirty()noexcept(!IS_DEBUG);
			bool IsDirty()const noexcept(!IS_DEBUG);

			DirectX::XMMATRIX GetTransform() const noexcept;
			void ApplyParameters() noexcept;
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
			struct NodeData
			{
				TransformParameters transformParams;
				bool transformParamsDirty = false;
				std::optional<MP::Buffer>materialCbuf;
				bool materialCbufDirty = false;
			};
			/*Node::PSMaterialConstantFullmonte _mSkinMaterial = {};
			Node::PSMaterialConstantNotex _mRingMaterial = {};*/
			std::unordered_map<int, NodeData>_mTransforms = {};
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