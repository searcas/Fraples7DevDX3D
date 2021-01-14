#include "Sync.h"
#include <algorithm>

namespace FraplesDev
{
	const std::string& Sync::GetRegisteredName() const noexcept
	{
		return _mRegisteredName;
	}
	const std::string& Sync::GetPassName() const noexcept
	{
		return _mPassName;
	}
	const std::string& Sync::GetSourceName() const noexcept
	{
		return _mOutputName;
	}
	void Sync::SetTarget(std::string passName, std::string outputName)
	{
		{
			if (passName.empty())
			{
				throw RGC_EXCEPTION("Empty output name");
			}
			const bool nameCharsValid = std::all_of(passName.begin(), passName.end(), [](char c) {return std::isalnum(c) || c == '_'; });
			if (passName != "$" && (!nameCharsValid || std::isdigit(passName.front())))
			{
				throw RGC_EXCEPTION("Invalid output name: " + _mRegisteredName);
			}
			_mPassName = passName;
		}
		{
			if (outputName.empty())
			{
				throw RGC_EXCEPTION("Empty output name");
			}
			const bool nameCharsValid = std::all_of(_mOutputName.begin(), _mOutputName.end(), [](char c) {return std::isalnum(c) || c == '_'; });
			if (!nameCharsValid || isdigit(outputName.front()))
			{
				throw RGC_EXCEPTION("Invalid output name: " + _mRegisteredName);
			}
			_mOutputName = outputName;
		}
	}

	Sync::Sync(std::string registeredName)
		: _mRegisteredName(std::move(registeredName))
	{
		if (_mRegisteredName.empty())
		{
			throw RGC_EXCEPTION("Empty output name");
		}
		const bool nameCharsValid = std::all_of(_mRegisteredName.begin(), _mRegisteredName.end(), [](char c) { return std::isalnum(c) || c == '_'; });
		if (!nameCharsValid || std::isdigit(_mRegisteredName.front()))
		{
			throw RGC_EXCEPTION("Invalid output name: "+ _mRegisteredName);
		}
	}

}