#pragma once

namespace FraplesDev
{
	class BufferResource
	{
	public:
	virtual	~BufferResource() = 0;
	virtual void BindAsBuffer(class Graphics&) noexcept = 0;
	virtual void BindAsBuffer(class Graphics&, BufferResource*) noexcept = 0;
	virtual void Clear(Graphics&)  noexcept = 0;

	private:

	};
}
