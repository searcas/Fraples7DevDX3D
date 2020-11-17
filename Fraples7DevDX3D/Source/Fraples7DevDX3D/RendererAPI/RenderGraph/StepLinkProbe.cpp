#include "StepLinkProbe.h"
namespace FraplesDev
{
	void StepLinkProbe::OnSetStep()
	{
		_mPstep->Link(rg);
	}
}