#pragma once
#include <vector>
#include <string>


namespace FraplesDev
{
	class Utility
	{
	public:
		static std::vector<std::string> TokenizeQuoted(const std::string& input);
		static std::wstring ToWide(const std::string& narrow);
		static std::string ToNarrow(const std::wstring& wide);

	private:

	};



}