#pragma once
#include "Core/MetaProgramming/DynamicConstant.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>

namespace FraplesDev
{
	class LayoutCodex
	{
	public:
		static bool Has(const std::string& tag)
		{
			return Get_().Has_(tag);
		}
		static MP::Layout Load(const std::string& tag)
		{
			return Get_().Load_(tag);
		}
		static void Store(const std::string& tag, MP::Layout& layout)
		{
			Get_().Store_(tag, layout);
		}
	private:
		static LayoutCodex& Get_()
		{
			static LayoutCodex codex;
			return codex;
		}
		bool Has_(const std::string& tag)const
		{
			return map.find(tag) != map.end();
		}
		MP::Layout Load_(const std::string& tag)const
		{
			return { map.find(tag)->second };
		}
		void Store_(const std::string& tag, MP::Layout& layout)
		{
			auto r = map.insert({ tag,layout.Finalize() });
			assert(r.second);
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<MP::LayoutElement>>map;
	};

}