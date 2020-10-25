#include "../Core/Fraples7.h"
#include "MouseEvents.h"


namespace FraplesDev {
	std::pair<int, int> Mouse::GetPos() const noexcept
	{
		return { _mX,_mY };
	}

	std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
	{
		if (_mRawDeltaBuffer.empty())
		{
			return std::nullopt;
		}
		const RawDelta del = _mRawDeltaBuffer.front();
		_mRawDeltaBuffer.pop();
		return del;
	}

	int Mouse::GetPosX() const noexcept
	{
		return _mX;
	}

	int Mouse::GetPosY() const noexcept
	{
		return _mY;
	}

	bool Mouse::IsInWindow() const noexcept
	{
		return _mIsInWindow;
	}

	bool Mouse::LeftIsPressed() const noexcept
	{
		return _mLeftIsPressed;
	}

	bool Mouse::RightIsPressed() const noexcept
	{
		return _mRightIsPressed;
	}

	std::optional<Mouse::Event> Mouse::Read() noexcept
	{
		if (_mBuffer.size() > 0u)
		{
			Mouse::Event ev = _mBuffer.front();
			_mBuffer.pop();
			return ev;
		}
		return {};
	}

	void Mouse::Flush() noexcept
	{
		_mBuffer = std::queue<Event>();
	}

	void Mouse::EnableRaw() noexcept
	{
		_mRawEnabled = true;
	}

	void Mouse::DisableRaw() noexcept
	{
		_mRawEnabled = false;
	}

	bool Mouse::RawEnabled() const noexcept
	{
		return _mRawEnabled;
	}

	void Mouse::OnMouseMove(int x, int y) noexcept
	{
		_mX = x;
		_mY = y;
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseLeave() noexcept
	{
		_mIsInWindow = false;
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
		TrimBuffer();
	}

	void Mouse::OnMouseEnter() noexcept
	{
		_mIsInWindow = false;
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
		TrimBuffer();
	}

	void Mouse::OnRawDelta(int dx, int dy) noexcept
	{
		_mRawDeltaBuffer.push({ dx, dy });
		TrimBuffer();
	}

	void Mouse::OnLeftPressed(int x, int y) noexcept
	{
		_mLeftIsPressed = true;
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));;
		TrimBuffer();
	}

	void Mouse::OnLeftReleased(int x, int y) noexcept
	{
		_mLeftIsPressed = false;
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));;
		TrimBuffer();
	}

	void Mouse::OnRightPressed(int x, int y) noexcept
	{
		_mRightIsPressed = true;
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));;
		TrimBuffer();
	}

	void Mouse::OnRightReleased(int x, int y) noexcept
	{
		_mRightIsPressed = false;
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));;
		TrimBuffer();
	}

	void Mouse::OnWheelUp(int x, int y) noexcept
	{
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
		TrimBuffer();
	}

	void Mouse::OnWheelDown(int x, int y) noexcept
	{
		_mBuffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
		TrimBuffer();
	}

	void Mouse::TrimBuffer() noexcept
	{
		while (_mBuffer.size() > _mBufferSize)
		{
			_mBuffer.pop();
		}
	}

	void Mouse::TrimRawInputBuffer() noexcept
	{
		while (_mRawDeltaBuffer.size() > _mBufferSize)
		{
			_mRawDeltaBuffer.pop();
		}
	}

	void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
	{
		_mWheelDeltaCarry += delta;

		while (_mWheelDeltaCarry >= WHEEL_DELTA)
		{
			_mWheelDeltaCarry -= WHEEL_DELTA;
			OnWheelUp(x, y);
		}
		while (_mWheelDeltaCarry <= -WHEEL_DELTA)
		{
			_mWheelDeltaCarry += WHEEL_DELTA;
			OnWheelDown(x, y);
		}
	}
}