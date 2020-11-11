#include "Technique.h"
#include "RendererAPI/Renderer.h"
#include "FrameCommander.h"
namespace FraplesDev
{
	void Technique::Submit(FrameCommander& frame, const Renderer& renderer) const noexcept
	{
		if (_mActivate)
		{
			for (const auto& step : _mSteps)
			{
				step.Submint(frame, renderer);
			}
		}
	}
	void Technique::InitializeParentReferences(const Renderer& parent) noexcept
	{
		for (auto& s : _mSteps)
		{
			s.InitializeParentReferences(parent);
		}
	}
}