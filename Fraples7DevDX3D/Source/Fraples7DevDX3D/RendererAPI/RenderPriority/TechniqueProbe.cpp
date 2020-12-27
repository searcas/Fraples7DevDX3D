#include "TechniqueProbe.h"
#include "Step.h"


namespace FraplesDev
{
	// TODO: Add callback for visiting each mesh
	void TechniqueProbe::SetTechnique(Technique* pTech_in)
	{
		_mPtech = pTech_in;
		_mTechIdx++;
		OnSetTechnique();
	}
	void TechniqueProbe::SetStep(Step* step)
	{
		_mStep = step;
		_mStepIdx++;
		OnSetStep();
	}
	bool TechniqueProbe::VisitBuffer(MP::Buffer& buf)
	{
		_mBufIdx++;
		return OnVisitBuffer(buf);
	}
	bool TechniqueProbe::OnVisitBuffer(MP::Buffer&)
	{
		return false;
	}
}