#include "Window.h"
#include <sstream>
#include "../../Core/Exceptions/WindowsThrowMacros.h"

namespace FraplesDev {
	Window::WindowClass Window::WindowClass::_sWindClass;
	//Window Exception Stuff
	std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
	{
		char* pMsgBuf = nullptr;
		//Windows will allocate memory for error string and make our pointer point to it
		const DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);

		//0 string length returned idicates to failure
		if (nMsgLen == 0)
		{
			return "Unidentified error code.";
		}
		//copy error string from windows-allocated buffer to std::string
		std::string errorString = pMsgBuf;
		//free windows buffer
		LocalFree(pMsgBuf);
		return errorString;
	}

	Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
		: Exception(line, file), _mHr(hr)
	{

	}

	const char* Window::HrException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec <<
			" ( " << (unsigned long)GetErrorCode() << " )" << std::endl <<
			"[Description] " << GetErrorDescription() << std::endl << GetOriginString();
		_mWhatBuffer == oss.str();
		return _mWhatBuffer.c_str();
	}

	const char* Window::HrException::GetType() const noexcept
	{
		return "Fraples Window Exception";
	}

	HRESULT Window::HrException::GetErrorCode() const noexcept
	{
		return _mHr;
	}

	std::string Window::HrException::GetErrorDescription() const noexcept
	{
		return Exception::TranslateErrorCode(_mHr);
	}

	const char* Window::NoGfxException::GetType() const noexcept
	{
		return "Fraples Window Exception [No Graphics]";

	}

	const char* Window::WindowClass::GetName() noexcept
	{
		return _mWndClassName;
	}

	HINSTANCE Window::WindowClass::GetInstance() noexcept
	{
		return _sWindClass.hInst;
	}

	Window::WindowClass::WindowClass() noexcept
	{
		const auto pClassName = "Fraples7 Engine Dx3D";
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = HandleMsgSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetInstance();
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = GetName();
		wc.hIconSm = nullptr;
		RegisterClassEx(&wc);
	}

	Window::WindowClass::~WindowClass()
	{
		UnregisterClass(GetName(), GetInstance());
	}

	Window::Window(int width, int height, const char* name)
		:_mWidth(width), _mHeight(height)
	{
		RECT wr;
		wr.left = 100;
		wr.right = width + wr.left;
		wr.top = 100;
		wr.bottom = height + wr.top;
		if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0);
		{
			throw FPL_LAST_EXCEPT();
		}
		// create window & get hwnd
		_mHwnd = CreateWindow(WindowClass::GetName(), name,
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
			wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr,
			WindowClass::GetInstance(), this);
		//check for error
		if (_mHwnd == nullptr)
		{
			throw FPL_LAST_EXCEPT();
		}
		ShowWindow(_mHwnd, SW_SHOWDEFAULT);

	}

	Window::~Window()
	{
		DestroyWindow(_mHwnd);
	}

	LRESULT Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
		if (msg == WM_NCCREATE)
		{
			// extract ptr to window class from creation data
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			// set WinAPI-managed user data to store ptr to window instance
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// set message proc to normal (non-setup) handler now that setup is finished
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
			//forward message to window instance handler
			return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
		}
		//if we get a message before the WM_NCCREATE message, handle with default handler

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//retrieve ptr to window isntance
		Window* const pwnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		//forward messsage to window instance handler
		return pwnd->HandleMsg(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) noexcept
	{
		switch (msg)
		{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wParam, lparam);
	}
}