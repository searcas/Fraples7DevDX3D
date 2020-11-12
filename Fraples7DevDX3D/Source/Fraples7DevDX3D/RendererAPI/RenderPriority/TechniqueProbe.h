#pragma once

namespace FraplesDev
{
	namespace MP
	{
		class Buffer;
	}
	class TechniqueProbe
	{
	public:
		
		void SetTechnique(class Technique* pTech_in)
		{
			_mPtech = pTech_in;
			_mTechIdx++;
			OnSetTechnique();
		}
		void SetStep(class Step* pStep_in)
		{
			_mPstep = pStep_in;
			_mStepIdx++;
			OnSetStep();
		}
		virtual bool VisitBuffer(class MP::Buffer&buf)
		{
			_mBufIdx++;
			return OnVisitBuffer(buf);
		}
		virtual bool OnVisitBuffer( class MP::Buffer&) = 0;
	protected:
		virtual void OnSetTechnique() {}
		virtual void OnSetStep() {}
		class Technique* _mPtech = nullptr;
		class Step* _mPstep = nullptr;
		size_t _mTechIdx = std::numeric_limits<size_t>::max();
		size_t _mStepIdx = std::numeric_limits<size_t>::max();
		size_t _mBufIdx = std::numeric_limits<size_t>::max();
	};
}