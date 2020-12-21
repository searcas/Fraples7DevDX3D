#pragma once
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
		void Submit() const noexcept(!IS_DEBUG);
		std::unique_ptr<Node>ParseNode(int& nextId,const aiNode& node, float scale)noexcept;
		void SetRootTransform(DirectX::FXMMATRIX tf);
		void Accept(class ModelProbe& probe);
		~Model();
	private:
		std::unique_ptr<Node>_mRoot;
		// sharing meshes here prehaps dangerous?
		std::vector<std::unique_ptr<Mesh>>_mMeshPtrs;

	};


}