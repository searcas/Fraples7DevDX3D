#include "Fraples7Utility.h"
#include <sstream>
#include <iomanip>

namespace FraplesDev
{
	std::vector<std::string> FraplesDev::Utility::TokenizeQuoted(const std::string& input)
	{
		std::istringstream stream;
		stream.str(input);
		std::vector<std::string>tokens;
		std::string token;
		while (stream >> std::quoted(token))
		{
			tokens.push_back(std::move(token));
		}

		return tokens;
	}

	std::wstring FraplesDev::Utility::ToWide(const std::string& narrow)
	{
		wchar_t wide[512];
		mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
		return wide;
	}

	std::string FraplesDev::Utility::ToNarrow(const std::wstring& wide)
	{
		char narrow[512];
		wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);
		return narrow;
	}
}