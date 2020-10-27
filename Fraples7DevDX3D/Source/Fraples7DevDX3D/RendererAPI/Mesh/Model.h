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
namespace FraplesDev
{
	

	class Node
	{
		friend class Model;
	public:
		Node(int id,const std::string& name,std::vector<Mesh*>meshPtrs, const DirectX::XMMATRIX& transform)noexcept(!IS_DEBUG);
		void Render(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform)const noexcept(!IS_DEBUG);
		void SetAppliedTransform(DirectX::FXMMATRIX transform)noexcept;
		void RenderTree(std::optional<int>& selectedIndex,Node*& pSelectedNode) const noexcept;
		inline const int GetId()const noexcept
		{
			return _mID;
 		}
	private:
		void AddChild(std::unique_ptr<Node>pChild)noexcept(!IS_DEBUG);
	private:
		int _mID;
		std::string _mName;
		std::vector<std::unique_ptr<Node>>_mChildPtrs;
		std::vector<Mesh*>_mMeshPtrs;
		DirectX::XMFLOAT4X4 _mTransform;
		DirectX::XMFLOAT4X4 appliedTransform;
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
			std::string note;
		};
		class ModelWindow
		{
		public:
			void Show(const char* windowName, const Node& root);

			DirectX::XMMATRIX GetTransform() const noexcept;
			Node* GetSelectedNode()	const noexcept;
		private:
			Node* _mPselectedNode;
			struct TransformParameters
			{
				float roll = 0.0f;
				float pitch = 0.0f ;
				float yaw = 0.0f;
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
			};
			std::unordered_map<int, TransformParameters>_mTransforms;
			std::optional<int>_mSelectedIndex;
		};

		Model(Graphics& gfx, const std::string fileName);
		void Render(Graphics& gfx) const;
		static std::unique_ptr<Mesh>ParseMesh(Graphics& gfx, const aiMesh& mesh);
		std::unique_ptr<Node>ParseNode(int& nextId,const aiNode& node)noexcept;
		
		~Model();
		void ShowModelInfo(const char* windowName);
	private:
		std::unique_ptr<Node>_mRoot;
		std::vector<std::unique_ptr<Mesh>>_mMeshPtrs;
		std::unique_ptr<class ModelWindow>_mpWindow;

	};


}