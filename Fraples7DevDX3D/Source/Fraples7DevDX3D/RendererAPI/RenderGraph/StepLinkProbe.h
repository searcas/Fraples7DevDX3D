#pragma once
#include "RendererAPI/RenderPriority/TechniqueProbe.h"
namespace FraplesDev
{
	class StepLinkProbe : public TechniqueProbe
	{
	protected:
		void OnSetStep()override;
	private:
		class RenderGraph& rg;
	};
}