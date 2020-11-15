#pragma once
#include <filesystem>
#include <vector>
#include <memory>
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/RenderPriority/Technique.h"
#include "Core/MetaProgramming/DynamicConstant.h"
#include "RendererAPI/ConstantBuffersEx.h"
#include "RendererAPI/VertexBuffer.h"
#include "RendererAPI/IndexBuffer.h"

namespace FraplesDev
{

	class Material
	{
	public:
		Material() = default;
		Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& modelPath)noexcept(!IS_DEBUG);
		
		MP::VertexBuffer ExtractVertices(const aiMesh& mesh)const noexcept;
		std::vector<Technique>GetTechniques()const noexcept;
		std::vector<unsigned short>ExtractIndices(const aiMesh& mesh)const noexcept;
		std::shared_ptr<VertexBuffer>MakeVertexContext(Graphics& gfx, const aiMesh& mesh, float scale = 1.0f)const noexcept(!IS_DEBUG);
		std::shared_ptr<IndexBuffer>MakeIndexContext(Graphics& gfx, const aiMesh& mesh)const noexcept(!IS_DEBUG);
	private:
		std::string MakeMeshTag(const aiMesh& mesh)const noexcept;
	private:
		MP::VertexLayout _mVertexLayout;
		std::vector<Technique>_mTechniques;
		std::string _mModelPath;
		std::string _mName;
	};
}