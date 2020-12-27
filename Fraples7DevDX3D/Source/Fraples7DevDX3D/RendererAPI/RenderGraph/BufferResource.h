#pragma once

namespace FraplesDev
{
	class Graphics;
	class BufferResource
	{
	public:
		virtual	~BufferResource() = 0;
		virtual void BindAsBuffer(Graphics&) noexcept = 0;
		virtual void BindAsBuffer(Graphics&, BufferResource*) noexcept = 0;
		virtual void Clear(Graphics& gfx)const noexcept = 0;
	};
}
