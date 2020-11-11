#include "Step.h"
#include "Job.h"
#include "RendererAPI/Renderer.h"

namespace FraplesDev
{
	Job::Job(const Step* pStep, const Renderer* pRenderer) 
		: _mPrenderer{ pRenderer },_mpStep(pStep)
	{
	}
	void Job::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		_mPrenderer->Bind(gfx);
		_mpStep->Bind(gfx);
		gfx.RenderIndexed(_mPrenderer->GetIndexCount());
	}
}