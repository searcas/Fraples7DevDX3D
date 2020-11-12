#pragma once
#include <assimp/scene.h>
#include <filesystem>
#include <vector>
#include "RendererAPI/GFXContextBase.h"
#include "RendererAPI/RenderPriority/Technique.h"

namespace FraplesDev
{
	class Material
	{
	public:
		Material(Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path)noexcept(!IS_DEBUG)
		{

		}
		MP::VertexBuffer ExtractVertices(const aiMesh& mesh)const noexcept
		{
			using type = MP::ElementType;
			MP::VertexBuffer buf{ _mVertexLayout };
			buf.Resize(mesh.mNumVertices);
			if (_mVertexLayout.Has<type::Position3D>())
			{
				for (int i = 0; i < mesh.mNumVertices; i++)
				{
					buf[i].
				}
			}
		}
		std::vector<Technique>GetTechniques()const noexcept
		{
			return _mTechniques;
		}
	private:
		MP::VertexLayout _mVertexLayout;
		std::vector<Technique>_mTechniques;
	};
}