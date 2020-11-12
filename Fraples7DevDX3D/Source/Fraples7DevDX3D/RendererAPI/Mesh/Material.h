#pragma once
#include <assimp/scene.h>
#include <filesystem>
#include <vector>
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/RenderPriority/Technique.h"
#include "Core/MetaProgramming/DynamicConstant.h"
#include "RendererAPI/ConstantBuffersEx.h"
namespace FraplesDev
{
	
	class Material
	{
	public:
		Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& modelPath)noexcept(!IS_DEBUG);
		
		MP::Vertex ExtractVertices(const aiMesh& mesh)const noexcept;
		std::vector<Technique>GetTechniques()const noexcept;
		std::vector<unsigned short>ExtractIndices(const aiMesh& mesh)const noexcept;
		std::shared_ptr<VertexBuffer>MakeVertexContext(Graphics& gfx, const aiMesh& mesh)const noexcept(!IS_DEBUG);
		std::shared_ptr<IndexBuffer>MakeIndexxContext(Graphics& gfx, const aiMesh& mesh)const noexcept(!IS_DEBUG);
	private:
		std::string MakeMeshTag(const aiMesh& mesh)const noexcept;
	private:
		MP::VertexLayout _mVertexLayout;
		std::vector<Technique>_mTechniques;
		std::string _mModelPath;
		std::string _mName;
	};
}