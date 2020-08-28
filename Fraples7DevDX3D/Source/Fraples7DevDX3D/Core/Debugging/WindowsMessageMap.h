#pragma once
#include <unordered_map>
#include "../Fraples7.h"


namespace FraplesDev {

	class WindowsMessageMap
	{
	public:
		WindowsMessageMap() noexcept;
		~WindowsMessageMap() = default;
		std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const noexcept;

	private:
		std::unordered_map<DWORD, std::string>_mMap;
	};

}