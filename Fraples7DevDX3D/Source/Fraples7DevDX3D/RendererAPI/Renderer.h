#pragma once
#include "RenderPriority/Technique.h"
#include <memory>
#include <DirectXMath.h>
#include "RenderPriority/Technique.h"
namespace FraplesDev
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class Inputlayout;

	class Renderer
	{

	public:
		Renderer() = default;
		Renderer(const Renderer&) = delete;
		void AddTechnique(Technique tech_in)noexcept;
		virtual const DirectX::XMMATRIX GetTransformXM()const noexcept = 0;
		void Submit(class FrameCommander& frame)const noexcept;
		void Bind(Graphics& gfx)const noexcept;
		UINT GetIndexCount()const noexcept(!IS_DEBUG);
		virtual ~Renderer() = default;
	protected:
		std::shared_ptr<IndexBuffer>_mPindices;
		std::shared_ptr<VertexBuffer>_mPvertices;
		std::shared_ptr<Topology>_mPtopology;
		std::vector<Technique>_mTechniques;
	};
}