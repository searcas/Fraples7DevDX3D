#pragma once
#include "../../Core/Fraples7.h"
#include "../../Core/Exceptions/FraplesException.h"

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
			HRESULT GetErrorCode()const noexcept;
			std::string GetErrorDescription() const noexcept;
		private:
			HRESULT _mHr;
		};
		class NoGfxException : public Exception
		{
		public:
			using Exception::Exception;
			const char* GetType()const noexcept override;
		private:

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
		Window(int width, int height, const char* name);
		~Window();
		Window(const Window&) = delete;
		Window& operator =(const Window&) = delete;

	private:
		static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) noexcept;
	private:
		int _mWidth;
		int _mHeight;
		HWND _mHwnd;
	};
}