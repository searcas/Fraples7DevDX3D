#include "StepLinkProbe.h"
namespace FraplesDev
{
	void StepLinkProbe::OnSetStep()
	{
		_mStep->Link(rg);
	}
}