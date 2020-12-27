#include "Step.h"
#include "RendererAPI/Renderer.h"

namespace FraplesDev
{
	Step::Step(std::string targetPassName)
		:_mTargetPassName{ std::move(targetPassName)}
	{

	}
	void Step::AddContext(std::shared_ptr<GfxContext>context)
	{
		_mPcontexts.push_back(std::move(context));
	}
	void Step::Bind(Graphics& gfx)const
	{
		for (auto& b : _mPcontexts)
		{
			b->Bind(gfx);
		}
	}
	void Step::Accept(TechniqueProbe& probe)
	{
		probe.SetStep(this);

		for (auto& pb : _mPcontexts)
		{
			pb->Accept(probe);
		}
	}
	Step::Step(const Step& src)noexcept
		:_mTargetPassName(src._mTargetPassName)
	{
		_mPcontexts.reserve(src._mPcontexts.size());
		for (auto& pb : src._mPcontexts)
		{
			if (auto* pCloning = dynamic_cast<const CloningContext*>(pb.get()))
			{
				_mPcontexts.push_back(pCloning->Clone());
			}
			else
			{
				_mPcontexts.push_back(pb);
			}
		}
	}
	void Step::Submit(const Renderer& renderer) const
	{
		_mTargetPass->Accept(Job{ this,&renderer });
	}
	void Step::InitializeParentReferences(const Renderer& parent) noexcept
	{
		for (auto& b : _mPcontexts)
		{
			b->InitializeParentReference(parent);
		}
	}
	void Step::Link(RenderGraph& rg)
	{
		assert(_mTargetPass == nullptr);
		_mTargetPass = &rg.GetRenderQueue(_mTargetPassName);
	}
}