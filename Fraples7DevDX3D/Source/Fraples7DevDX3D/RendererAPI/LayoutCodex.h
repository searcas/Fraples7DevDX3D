#pragma once
#include "Core/MetaProgramming/DynamicConstant.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>

namespace FraplesDev
{
	namespace MP
	{
		class LayoutCodex
		{
		public:
			static MP::CookedLayout Resolve(MP::RawLayout&& layout)noexcept(!IS_DEBUG);
		private:
			static LayoutCodex& Get_()noexcept;
		private:
			std::unordered_map<std::string, std::shared_ptr<MP::LayoutElement>>map;
		};
	}
}