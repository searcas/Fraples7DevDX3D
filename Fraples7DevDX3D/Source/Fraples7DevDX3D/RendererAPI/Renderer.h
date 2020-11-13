#pragma once
#include <memory>
#include <DirectXMath.h>
#include "assimp/scene.h"
#include "RenderPriority/Technique.h"
#include "assimp/scene.h"
namespace FraplesDev
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class Inputlayout;

	class Renderer
	{

	public:
		Renderer(Graphics& gfx, const class Material& mat, const aiMesh& mesh)noexcept;
		Renderer() = default;
		Renderer(const Renderer&) = delete;
		void AddTechnique(Technique tech_in)noexcept;
		virtual const DirectX::XMMATRIX GetTransformXM()const noexcept = 0;
		void Submit(class FrameCommander& frame)const noexcept;
		void Bind(Graphics& gfx)const noexcept;
		UINT GetIndexCount()const noexcept(!IS_DEBUG);
		void Accept(class TechniqueProbe& probe);

		virtual ~Renderer() = default;
	protected:
		std::shared_ptr<IndexBuffer>_mPindices;
		std::shared_ptr<VertexBuffer>_mPvertices;
		std::shared_ptr<Topology>_mPtopology;
		std::vector<Technique>_mTechniques;
	};
}