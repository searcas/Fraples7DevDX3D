#include "PassOutput.h"
namespace FraplesDev
{
	const std::string& PassOutput::GetName() const noexcept
	{
		// TODO: insert return statement here
	}
	void PassOutput::PostLinkValidation() const
	{
	}
	std::shared_ptr<class GfxContext> PassOutput::YieldImmutable()
	{
		return std::shared_ptr<class GfxContext>();
	}
	std::shared_ptr<class BufferResource> PassOutput::YieldBuffer()
	{
		return std::shared_ptr<class BufferResource>();
	}
	PassOutput::PassOutput(std::string name)
	{
	}
}