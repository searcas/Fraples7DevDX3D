#pragma once
#include "../GraphicAPI/Graphic.h"
#include "GFXContext.h"
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
		void AddBind(std::unique_ptr<class GfxContext>bind) noexcept;
		void AddIndexBuffer(std::unique_ptr<class IndexBuffer>ibuf)noexcept;
	public:
		virtual DirectX::XMMATRIX GetTransformXM()const noexcept = 0;
		virtual void Update(float diff) noexcept = 0;
		virtual ~Renderer() = default;
	private:
		const IndexBuffer* _mpIndexBuffer = nullptr;
		std::vector<std::unique_ptr<class GfxContext>>_mBinds;
	};
}