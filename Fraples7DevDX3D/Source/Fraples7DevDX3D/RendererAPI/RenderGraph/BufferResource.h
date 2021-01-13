#pragma once

namespace FraplesDev
{
	class Graphics;
	class BufferResource
	{
	public:
		virtual	~BufferResource() = default;
		virtual void BindAsBuffer(Graphics&) noexcept(!IS_DEBUG) = 0;
		virtual void BindAsBuffer(Graphics&, BufferResource*) noexcept(!IS_DEBUG) = 0;
		virtual void Clear(Graphics& gfx)const noexcept(!IS_DEBUG) = 0;
	};
}
