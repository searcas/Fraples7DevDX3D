#include "LayoutCodex.h"



namespace FraplesDev
{
	namespace MP
	{
		MP::CookedLayout LayoutCodex::Resolve(MP::RawLayout&& layout) noexcept(!IS_DEBUG)
		{
			auto sig = layout.GetSignature();
			auto& map = Get_().map;
			const auto i = map.find(sig);
			//identical layout already exists
			if (i != map.end())
			{
				// Input layout is expected to be cleared after Resolve
				// so just throw away the layout tree
				layout.ClearRoot();
				return { i->second };
			}
			// Otherwise add layout root element to map
			auto result = map.insert({ std::move(sig), layout.DeliverRoot() });
			
			//return layout with additional reference to root
			return { result.first->second };
		}
		LayoutCodex& LayoutCodex::Get_()noexcept
		{
			static LayoutCodex codex;
			return codex;
		}
	}
}