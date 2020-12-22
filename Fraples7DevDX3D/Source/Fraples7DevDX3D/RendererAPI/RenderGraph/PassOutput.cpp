#include "PassOutput.h"
#include "BufferResource.h"
#include "GraphicAPI/Graphics.h"
#include <algorithm>
namespace FraplesDev
{
	const std::string& PassOutput::GetName() const noexcept
	{
		return _mName;
	}
	std::shared_ptr<GfxContext> PassOutput::YieldImmutable()
	{
		throw RGC_EXCEPTION("Output cannot be accessed as immutable");
	}
	std::shared_ptr<BufferResource> PassOutput::YieldBuffer()
	{
		throw RGC_EXCEPTION("Output cannot be accessed as buffer");
	}
	PassOutput::PassOutput(std::string name)
		:_mName(std::move(name))
	{
		if (_mName.empty())
		{
			throw RGC_EXCEPTION("Empty output name");
		}
		const bool nameCharsValid = std::all_of(_mName.begin(), _mName.end(), [](char c) {std::isalnum(c) || c == '_'; });
		if (!nameCharsValid || std::isdigit(name.front))
		{
			throw RGC_EXCEPTION("Invalid output name" + _mName);
		}
	}
}