#pragma once
#include "../GraphicAPI/Graphics.h"
#include "GFXContext.h"
#include "IndexBuffer.h"
#include <DirectXMath.h>
#include <memory>

namespace FraplesDev
{

	class Renderer
	{
		template<typename T>
		friend class RendererBase;
	public:
		void Render(Graphics& gfx) const noexcept;
		Renderer() = default;
		Renderer(const Renderer&) = delete;
	protected:
		void AddBind(std::unique_ptr<class GfxContext>bind) noexcept(!IS_DEBUG);
		void AddIndexBuffer(std::unique_ptr<class IndexBuffer>ibuf)noexcept(!IS_DEBUG);
	public:
		virtual const DirectX::XMMATRIX GetTransformXM()const noexcept = 0;
		virtual void Update(float diff) noexcept = 0;
		virtual ~Renderer() = default;
		virtual const std::vector<std::unique_ptr<GfxContext>>& GetStaticBinds() const noexcept = 0;
	protected:
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
		std::vector<std::unique_ptr<class GfxContext>>_mBinds;
	};
}