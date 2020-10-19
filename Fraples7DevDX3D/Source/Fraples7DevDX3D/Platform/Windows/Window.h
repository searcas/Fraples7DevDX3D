#pragma once
#include "../../Core/Fraples7.h"
#include "../../Core/Debugging/Exceptions/FraplesException.h"
#include "../../Input Events/KeyboardEvent.h"
#include "../../Input Events/MouseEvents.h"
#include "../../GraphicAPI/Graphic.h"
#include "../../ImGuiMgr/ImGuiMgr.h"

#include <memory>
namespace FraplesDev {
	class Window
	{
	public:
		class Exception : public FraplesException
		{
			using FraplesException::FraplesException;
		public:
			static std::string TranslateErrorCode(HRESULT hr)noexcept;
		};
		class HrException : public Exception
		{
		public:
			HrException(int line, const char* file, HRESULT hr)noexcept;
			const char* what() const noexcept override;
			const char* GetType() const noexcept override;
			HRESULT GetErrorCode() const noexcept;
			std::string GetErrorDescription() const noexcept;
		private:
			HRESULT _mHr;
		};
		class NoGfxException : public Exception
		{
		public:
			using Exception::Exception;
			const char* GetType()const noexcept override;

		};
		//singleton manages registration/cleanup of window class
		class WindowClass
		{
		public:
			static const char* GetName() noexcept;
			static HINSTANCE GetInstance() noexcept;
		private:
			WindowClass() noexcept;
			~WindowClass();
			WindowClass(const WindowClass&) = delete;
			WindowClass& operator=(const WindowClass&) = delete;
			static constexpr const char* _mWndClassName = "Fraplse7 Direct3D Engine Window";
			static WindowClass _sWindClass;
			HINSTANCE hInst;
		};
	public:
		Window(const char* name, int width, int height);
		Window() = default;
		~Window();
		Window(const Window&) = delete;
		Window& operator =(const Window&) = delete;
		static std::optional<int>ProcessMessages()noexcept;
		Graphics& GetGFX();
	public:
		 Keyboard _mKey;
		 Mouse _mMouse;
	private:
		void ConfineCursor() noexcept;
		void FreeCursor() noexcept;
		void ShowCursor() noexcept;
		void HideCursor() noexcept;

		static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) noexcept;
	private:
		ImGuiMgr mgr;
		bool _mCursorEnabled = true;
		int _mWidth;
		int _mHeight;
		HWND _mHwnd;
		std::unique_ptr<Graphics>_mpGFX;
		std::vector<BYTE>_mRawBuffer;
	};
}