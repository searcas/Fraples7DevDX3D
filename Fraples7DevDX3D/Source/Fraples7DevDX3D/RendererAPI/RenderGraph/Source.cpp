#include "Source.h"
#include "BufferResource.h"
#include "GraphicAPI/Graphics.h"
#include <algorithm>
namespace FraplesDev
{
	const std::string& Source::GetName() const noexcept
	{
		return _mName;
	}
	std::shared_ptr<GfxContext> Source::YieldBindable()
	{
		throw RGC_EXCEPTION("Output cannot be accessed as immutable");
	}
	std::shared_ptr<BufferResource> Source::YieldBuffer()
	{
		throw RGC_EXCEPTION("Output cannot be accessed as buffer");
	}
	Source::Source(std::string name)
		:_mName(std::move(name))
	{
		if (_mName.empty())
		{
			throw RGC_EXCEPTION("Empty output name");
		}
		const bool nameCharsValid = std::all_of(_mName.begin(), _mName.end(), [](char c) {return std::isalnum(c) || c == '_'; });
		if (!nameCharsValid || std::isdigit(_mName.front()))
		{
			throw RGC_EXCEPTION("Invalid output name" + _mName);
		}
	}
}