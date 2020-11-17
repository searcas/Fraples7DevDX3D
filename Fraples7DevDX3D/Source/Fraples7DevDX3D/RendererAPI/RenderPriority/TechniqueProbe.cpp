#include "TechniqueProbe.h"



namespace FraplesDev
{

	// TODO: Add callback for visiting each mesh
	void TechniqueProbe::SetTechnique(class Technique* pTech_in)
	{
		_mPtech = pTech_in;
		_mTechIdx++;
		OnSetTechnique();
	}
	void TechniqueProbe::SetStep(class Step* pStep_in)
	{
		_mPstep = pStep_in;
		_mStepIdx++;
		OnSetStep();
	}
	bool TechniqueProbe::VisitBuffer(class MP::Buffer& buf)
	{
		_mBufIdx++;
		return OnVisitBuffer(buf);
	}
	bool TechniqueProbe::OnVisitBuffer(class MP::Buffer&)
	{
		return false;
	}

}