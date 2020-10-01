#pragma once
#include "Renderer.h"
#include "IndexBuffer.h"


namespace FraplesDev
{
	template<typename T>
	class RendererBase : public Renderer
	{
	protected:
		static bool IsStaticInitialized() noexcept
		{
			return !_sBinds.empty();
		}
		static void AddStaticBind(std::unique_ptr<GfxContext>bind)noexcept(!IS_DEBUG)
		{
			assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
			_sBinds.push_back(std::move(bind));
		}
		void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer>ibuf)noexcept(!IS_DEBUG)
		{
			assert("Attempting to add index buffer a second time " &&_mpIndexBuffer == nullptr);
			_mpIndexBuffer = ibuf.get();
			_sBinds.push_back(std::move(ibuf));
		}
		void SetIndexFromStatic()noexcept(!IS_DEBUG)
		{
			assert("Attempting to add index buffer a second time" && _mpIndexBuffer == nullptr);
			for (const auto& b : _sBinds)
			{
				if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
				{
					_mpIndexBuffer = p;
						return;
				}
			}
			assert("Failed to find index buffer in static binds" && _mpIndexBuffer != nullptr);

		}
	private:
		const std::vector<std::unique_ptr<GfxContext>>& GetStaticBinds()const noexcept override
		{
			return _sBinds;
		}

	private:
		static std::vector<std::unique_ptr<GfxContext>> _sBinds;
	};
	template<typename T>
	std::vector<std::unique_ptr<GfxContext>> RendererBase<T>::_sBinds;

}