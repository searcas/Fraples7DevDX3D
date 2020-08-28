#pragma once
#include <queue>
#include <optional>



namespace FraplesDev {
	class Mouse
	{
		friend class Window;
	public:
		struct RawDelta
		{
			int x, y;
		};
		class Event
		{
		public:
			enum class Type
			{
				LPress,
				LRelease,
				RPress,
				RRelease,
				WheelUp,
				WheelDown,
				Move,
				Enter,
				Leave,
			};
		private:
			Type _mType;
			bool _mLeftIsPressed;
			bool _mRightIsPressed;
			int _mX;
			int _mY;
		public:
			Event(Type type, const Mouse& parent) noexcept
				:
				_mType(type),
				_mLeftIsPressed(parent._mLeftIsPressed),
				_mRightIsPressed(parent._mRightIsPressed),
				_mX(parent._mX),
				_mY(parent._mY)
			{}
			Type GetType() const noexcept
			{
				return _mType;
			}
			std::pair<int, int> GetPos() const noexcept
			{
				return{ _mX,_mY };
			}
			int GetPosX() const noexcept
			{
				return _mX;
			}
			int GetPosY() const noexcept
			{
				return _mY;
			}
			bool LeftIsPressed() const noexcept
			{
				return _mLeftIsPressed;
			}
			bool RightIsPressed() const noexcept
			{
				return _mRightIsPressed;
			}
		};
	public:
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		std::pair<int, int> GetPos() const noexcept;
		std::optional<RawDelta> ReadRawDelta() noexcept;
		int GetPosX() const noexcept;
		int GetPosY() const noexcept;
		bool IsInWindow() const noexcept;
		bool LeftIsPressed() const noexcept;
		bool RightIsPressed() const noexcept;
		std::optional<Mouse::Event> Read() noexcept;
		bool IsEmpty() const noexcept
		{
			return _mBuffer.empty();
		}
		void Flush() noexcept;
		void EnableRaw() noexcept;
		void DisableRaw() noexcept;
		bool RawEnabled() const noexcept;
	private:
		void OnMouseMove(int x, int y) noexcept;
		void OnMouseLeave() noexcept;
		void OnMouseEnter() noexcept;
		void OnRawDelta(int dx, int dy) noexcept;
		void OnLeftPressed(int x, int y) noexcept;
		void OnLeftReleased(int x, int y) noexcept;
		void OnRightPressed(int x, int y) noexcept;
		void OnRightReleased(int x, int y) noexcept;
		void OnWheelUp(int x, int y) noexcept;
		void OnWheelDown(int x, int y) noexcept;
		void TrimBuffer() noexcept;
		void TrimRawInputBuffer() noexcept;
		void OnWheelDelta(int x, int y, int delta) noexcept;
	private:
		static constexpr unsigned int _mBufferSize = 16u;
		int _mX;
		int _mY;
		bool _mLeftIsPressed = false;
		bool _mRightIsPressed = false;
		bool _mIsInWindow = false;
		int _mWheelDeltaCarry = 0;
		bool _mRawEnabled = false;
		std::queue<Event> _mBuffer;
		std::queue<RawDelta> _mRawDeltaBuffer;
	};
}