#include "RenderQueuePass.h"

namespace FraplesDev
{
	void RenderQueuePass::Accept(Job job) noexcept
	{
		_mJobs.push_back(job);
	}
	void RenderQueuePass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
		BindAll(gfx);
		for (auto& j : _mJobs)
		{
			j.Execute(gfx);
		}
	}
	void RenderQueuePass::Reset() noexcept(!IS_DEBUG)
	{
		_mJobs.clear();
	}
}