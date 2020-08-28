#include "KeyboardEvent.h"

bool FraplesDev::Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return _mKeystates[keycode];
}

std::optional<FraplesDev::Keyboard::Event> FraplesDev::Keyboard::Readkey() noexcept
{
	if (_mKeybuffer.size() > 0u)
	{
		Keyboard::Event e = _mKeybuffer.front();
		_mKeybuffer.pop();
		return e;
	}
	return {};
}

bool FraplesDev::Keyboard::KeyIsEmpty() const noexcept
{
	return _mKeybuffer.empty();
}

void FraplesDev::Keyboard::FlushKey() noexcept
{
	_mKeybuffer = std::queue<Event>();
}

std::optional<char> FraplesDev::Keyboard::ReadChar() noexcept
{
	if (_mCharBuffer.size() > 0u)
	{
		unsigned char charcode = _mCharBuffer.front();
		_mCharBuffer.pop();
		return charcode;
	}
	return {};
}

bool FraplesDev::Keyboard::CharIsEmpty() const noexcept
{
	return _mCharBuffer.empty();
}

void FraplesDev::Keyboard::FlushChar() noexcept
{
	_mCharBuffer = std::queue<char>();
}

void FraplesDev::Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void FraplesDev::Keyboard::EnableAutorepeat() noexcept
{
	_mAutoRepeatEnabled = true;
}

void FraplesDev::Keyboard::DisableAutorepeat() noexcept
{
	_mAutoRepeatEnabled = false;

}

bool FraplesDev::Keyboard::AutorepeatIsEnabled() const noexcept
{
	return _mAutoRepeatEnabled;
}

void FraplesDev::Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	_mKeystates[keycode] = true;
	_mKeybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(_mKeybuffer);

}

void FraplesDev::Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	_mKeystates[keycode] = false;
	_mKeybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(_mKeybuffer);
}

void FraplesDev::Keyboard::OnChar(char character) noexcept
{
	_mCharBuffer.push(character);
	TrimBuffer(_mCharBuffer);
}

void FraplesDev::Keyboard::ClearState() noexcept
{
	_mKeystates.reset();
}

template<typename T>
void FraplesDev::Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > _mBufferSize)
	{
		buffer.pop();
	}
}