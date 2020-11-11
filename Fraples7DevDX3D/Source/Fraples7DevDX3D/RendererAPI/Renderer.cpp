#include "Renderer.h"
#include "GFXContextBase.h"
#include "GFXContextCodex.h"
namespace FraplesDev {
	
	void Renderer::AddTechnique(Technique tech_in) noexcept
	{
		tech_in.InitializeParentReferences(*this);
		_mTechniques.push_back(std::move(tech_in));
	}
	void Renderer::Submit(FrameCommander& frame) const noexcept
	{
		for (auto& tech : _mTechniques)
		{
			tech.Submit(frame, *this);
		}
	}
	void Renderer::Bind(Graphics& gfx)const noexcept
	{
		_mPtopology->Bind(gfx);
		_mPindices->Bind(gfx);
		_mPvertices->Bind(gfx);
	}
	UINT Renderer::GetIndexCount() const noexcept(!IS_DEBUG)
	{
		return _mPindices->GetCount();
	}

}