#pragma once
#include "RendererAPI/RenderPriority/Step.h"
#include <limits>
namespace FraplesDev
{
	class Technique;
	namespace MP
	{
		class Buffer;
	}
	class TechniqueProbe
	{
	public:
		// TODO: Add callback for visiting each mesh
		void SetTechnique(Technique* pTech_in);
		void SetStep(Step* pStep_in);
		virtual bool VisitBuffer(class MP::Buffer& buf);
		virtual bool OnVisitBuffer(class MP::Buffer&);
	protected:
		virtual void OnSetTechnique() {}
		virtual void OnSetStep() {}
		Technique* _mPtech = nullptr;
		Step* _mStep = nullptr;
		size_t _mTechIdx = std::numeric_limits<size_t>::max();
		size_t _mStepIdx = std::numeric_limits<size_t>::max();
		size_t _mBufIdx = std::numeric_limits<size_t>::max();
	};
}