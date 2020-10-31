#pragma once
#include "GFXContext.h"
#include "GraphicAPI/Graphics.h"
#include "IndexBuffer.h"
#include <DirectXMath.h>
#include <memory>

namespace FraplesDev
{

	class Renderer
	{

	public:
		void Render(Graphics& gfx) const noexcept;
		Renderer() = default;
		Renderer(const Renderer&) = delete;
	protected:
		void AddBind(std::shared_ptr<class GfxContext>bind) noexcept(!IS_DEBUG);
	public:
		virtual const DirectX::XMMATRIX GetTransformXM()const noexcept = 0;
		virtual ~Renderer() = default;
		template <class T>
		T* QueryBindable()noexcept
		{
			for (auto& pb :_mBinds)
			{
				if (auto pt = dynamic_cast<T*>(pb.get()))
				{
					return pt;
				}
			}
			return nullptr;
		}
	private:
		const IndexBuffer* _mpIndexBuffer = nullptr;
		std::vector<std::shared_ptr<class GfxContext>>_mBinds;
	};
}