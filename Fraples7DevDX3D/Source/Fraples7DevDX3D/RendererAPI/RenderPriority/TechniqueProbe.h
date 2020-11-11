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
			OnSetTechnique();
		}
		void SetStep(class Step* pStep_in)
		{
			_mPstep = pStep_in;
			OnSetStep();
		}
		virtual bool VisitBuffer( class MP::Buffer&) = 0;
	protected:
		virtual void OnSetTechnique() {}
		virtual void OnSetStep() {}
		class Technique* _mPtech = nullptr;
		class Step* _mPstep = nullptr;
	};
}