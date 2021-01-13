#pragma once

#include <memory>
#include <unordered_map>
#include <type_traits>


namespace FraplesDev
{
	class Graphics;
	class GfxContext;
	class Codex
	{
	public:
		template <class T, typename... Args>
		static std::shared_ptr<T>Resolve(Graphics& gfx, Args&& ...args)noexcept(IS_DEBUG)
		{
			static_assert(std::is_base_of<GfxContext,T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(gfx,std::forward<Args>(args)...);
		}

	private:
		template <class T, typename... Args>
		std::shared_ptr<T>Resolve_(Graphics& gfx,Args&&...args) noexcept(!IS_DEBUG)
		{
			const auto key = T::GenerateUID(std::forward<Args>(args)...);
			const auto i = _mBinds.find(key);
			if (i == _mBinds.end())
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Args>(args)...);
				_mBinds[key] = bind;
				return bind;
			}
			else
			{

				return std::static_pointer_cast<T>(i->second);
			}
		}

		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
		std::unordered_map<std::string, std::shared_ptr<GfxContext>>_mBinds;
	};
}