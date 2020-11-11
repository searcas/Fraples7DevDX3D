#pragma once
#include <vector>
#include <string>


namespace FraplesDev
{
	namespace Utility
	{
		  std::vector<std::string> TokenizeQuoted(const std::string& input);
		  std::wstring ToWide(const std::string& narrow);
		  std::string ToNarrow(const std::wstring& wide);

	};
}