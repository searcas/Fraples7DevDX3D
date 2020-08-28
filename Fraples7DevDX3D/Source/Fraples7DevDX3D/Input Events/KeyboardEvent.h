#pragma once

#include <optional>
#include <queue>
#include <bitset>
namespace FraplesDev
{
	class Keyboard
	{
		friend class Window;
	public:
		class Event
		{
		public:
			enum class Type
			{
				Press,
				Release
			};
		public:
			Event(Type type, unsigned char code)noexcept
				:_mType(type),_mCode(code)
			{

			}
			bool isPressed()const noexcept
			{
				return _mType == Type::Press;
			}
			bool isReleased()const noexcept
			{
				return _mType == Type::Release;
			}
			unsigned char GetCode()const noexcept
			{
				return _mCode;
			}
		private:
			Type _mType;
			unsigned char _mCode;
		};
	
	public:
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator= (const Keyboard&) = delete;
		//key event stuff
		bool KeyIsPressed(unsigned char keycode) const noexcept;
		std::optional<Event> Readkey() noexcept;
		bool KeyIsEmpty()const noexcept;
		void FlushKey() noexcept;
		//char event stuff
		std::optional<char>ReadChar() noexcept;
		bool CharIsEmpty()const noexcept;
		void FlushChar()noexcept;
		void Flush()noexcept;
		//autorepeat control
		void EnableAutorepeat() noexcept;
		void DisableAutorepeat() noexcept;
		bool AutorepeatIsEnabled() const noexcept;
	private:
		void OnKeyPressed(unsigned char keycode)noexcept;
		void OnKeyReleased(unsigned char keycode)noexcept;
		void OnChar(char character) noexcept;
		void ClearState() noexcept;
		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) noexcept;
	private:
		static constexpr unsigned int _mNKeys = 256u;
		static constexpr unsigned int _mBufferSize = 16u;
		bool _mAutoRepeatEnabled = false;
		std::bitset<_mNKeys>_mKeystates;
		std::queue<Event> _mKeybuffer;
		std::queue<char> _mCharBuffer;

	};




}