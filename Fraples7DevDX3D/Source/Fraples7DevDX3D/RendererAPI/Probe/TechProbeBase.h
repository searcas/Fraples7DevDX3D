#pragma once
#include "RendererAPI/RenderPriority/TechniqueProbe.h"

namespace FraplesDev
{

	namespace MP
	{
		class Buffer;
	}
	class TechProbe : public TechniqueProbe
	{
	public:
		void OnSetTechnique()override;
		bool OnVisitBuffer(MP::Buffer& buf);
	};
}