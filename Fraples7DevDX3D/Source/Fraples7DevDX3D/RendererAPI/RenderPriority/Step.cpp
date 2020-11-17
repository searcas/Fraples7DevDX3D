#include "Step.h"
#include "FrameCommander.h"
#include "RendererAPI/Renderer.h"

namespace FraplesDev
{
	Step::Step(size_t targetPass_in)
		:_mTargetPass{ targetPass_in }
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
		:_mTargetPass(src._mTargetPass)
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
	void Step::Submint(FrameCommander& frame, const Renderer& renderer) const
	{
		frame.Accept(Job{ this,&renderer }, _mTargetPass);
	}
	void Step::InitializeParentReferences(const Renderer& parent) noexcept
	{
		for (auto& b : _mPcontexts)
		{
			b->InitializeParentReference(parent);
		}
	}
}