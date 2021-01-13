#include "Renderer.h"
#include "GFXContextBase.h"
#include "GFXContextCodex.h"
#include "RendererAPI/Mesh/Material.h"

namespace FraplesDev {


	Renderer::Renderer(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept
	{
		_mPvertices = mat.MakeVertexContext(gfx, mesh,scale);
		_mPindices = mat.MakeIndexContext(gfx, mesh);
		_mPtopology = Topology::Resolve(gfx);

		for (auto& t : mat.GetTechniques())
		{
			AddTechnique(std::move(t));
		}
	}
	void Renderer::AddTechnique(Technique tech_in) noexcept
	{
		tech_in.InitializeParentReferences(*this);
		_mTechniques.push_back(std::move(tech_in));
	}
	void Renderer::Submit() const noexcept
	{
		for (auto& tech : _mTechniques)
		{
			tech.Submit(*this);
		}
	}
	void Renderer::Bind(Graphics& gfx)const noexcept(!IS_DEBUG)
	{
		_mPtopology->Bind(gfx);
		_mPindices->Bind(gfx);
		_mPvertices->Bind(gfx);
	}
	UINT Renderer::GetIndexCount() const noexcept(!IS_DEBUG)
	{
		return _mPindices->GetCount();
	}
	void Renderer::Accept(TechniqueProbe& probe)
	{
		for (auto& t : _mTechniques)
		{
			t.Accept(probe);
		}
	}
	void Renderer::LinkTechniques(RenderGraph& rg)
	{
		for (auto& tech : _mTechniques)
		{
			tech.Link(rg);
		}
	}
}