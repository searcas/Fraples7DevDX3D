#include "Step.h"
#include "FrameCommander.h"
#include "RendererAPI/Renderer.h"

namespace FraplesDev
{
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