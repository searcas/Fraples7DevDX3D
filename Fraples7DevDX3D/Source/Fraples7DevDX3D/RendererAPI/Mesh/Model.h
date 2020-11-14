#pragma once
#include "ModelWindow.h"
#include <memory>
#include <GraphicAPI/Graphics.h>
#include "Mesh.h"
namespace FraplesDev
{
	class Node;
	class Model
	{
	public:
		Model(Graphics& gfx, const std::string& path, float scale = 1.0f);
		void Submit(class FrameCommander& frame) const noexcept(!IS_DEBUG);
		static std::unique_ptr<Mesh>ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path,const float& scale);
		std::unique_ptr<Node>ParseNode(int& nextId,const aiNode& node, DirectX::FXMMATRIX additionalTransform)noexcept;
		void SetRootTransform(DirectX::FXMMATRIX tf);
		~Model();
	//	void ShowModelInfo(Graphics& gfx, const char* windowName);
	private:
		std::unique_ptr<Node>_mRoot;
		// sharing meshes here prehaps dangerous?
		std::vector<std::unique_ptr<Mesh>>_mMeshPtrs;
		//std::unique_ptr<class ModelWindow>_mpWindow;

	};


}