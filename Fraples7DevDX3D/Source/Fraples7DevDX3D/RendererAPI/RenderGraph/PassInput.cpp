#include "PassInput.h"
#include <algorithm>

namespace FraplesDev
{
	const std::string& PassInput::GetRegisteredName() const noexcept
	{
		return _mRegisteredName;
	}
	const std::string& PassInput::GetPassName() const noexcept
	{
		return _mPassName;
	}
	const std::string& PassInput::GetOutputName() const noexcept
	{
		return _mOutputName;
	}
	void PassInput::SetTarget(std::string passName, std::string outputName)
	{
		{
			if (passName.empty())
			{
				throw RGC_EXCEPTION("Empty output name");
			}
			const bool nameCharsValid = std::all_of(passName.begin(), passName.end(), [](char c) {return std::isalnum(c) || c == '_'; });
			if (_mPassName != "$" && (!nameCharsValid || std::isdigit(_mPassName.front())))
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
	void PassInput::Bind(PassOutput& out)
	{
	}
	void PassInput::PostLinkValidate() const
	{
	}
	PassInput::PassInput(std::string registeredName)
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