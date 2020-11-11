#include "Window.h"
#include <sstream>
#include "../../Core/Common/Exceptions/Macros/WindowsThrowMacros.h"
#include "../../../../ImGui/imgui_impl_win32.h"
#include "../Source/Fraples7DevDX3D/ImGuiMgr/ImGuiMgr.h"
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
		_mWhatBuffer = oss.str();
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
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = HandleMsgSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetInstance();
		wc.hIcon = reinterpret_cast<HICON>(LoadImage(hInst,GetName(),IMAGE_ICON,32,32,LR_DEFAULTSIZE));
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

	Window::Window(const char* name, int width, int height )
		:_mWidth(width), _mHeight(height)
	{
		RECT wr;
		wr.left = 100;
		wr.right = width + wr.left;
		wr.top = 100;
		wr.bottom = height + wr.top;
		if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
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
		ImGui_ImplWin32_Init(_mHwnd);
		_mpGFX = std::make_unique<Graphics>(_mHwnd,width,height);

		//register mouse raw input device
		RAWINPUTDEVICE rawInputDevice;
		rawInputDevice.usUsagePage = 0x01; //mouse page
		rawInputDevice.usUsage = 0x02; //mouse usage
		rawInputDevice.dwFlags = 0;
		rawInputDevice.hwndTarget = nullptr;
		if (RegisterRawInputDevices(&rawInputDevice,1,sizeof(rawInputDevice))==FALSE)
		{
			throw FPL_LAST_EXCEPT();
		}
	}


	Window::~Window()
	{
		ImGui_ImplWin32_Shutdown();
		DestroyWindow(_mHwnd);
	}
	std::optional<int>Window::ProcessMessages()noexcept
	{
		MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// check for quit because peekmessage does not signal this via return val
			if (msg.message == WM_QUIT)
			{
				// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
				return (int)msg.wParam;
			}
			// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
		}
		// return empty optional when not quitting app
		return {};
	}
	Graphics& Window::GetGFX()
	{
		if (!_mpGFX)
		{
			throw FPL_NOGFX_EXCEPT();
		}
		return *_mpGFX;
	}
	void Window::EnableCursor()noexcept
	{
		_mCursorEnabled = true;
		ShowCursor();
		EnableImGuiMouse();
		FreeCursor();
	}
	void Window::DisableCursor()noexcept
	{
		_mCursorEnabled = false;
		HideCursor();
		DisableImGuiMouse();
		ConfineCursor();
	}
	void Window::ConfineCursor() noexcept
	{
		RECT rect;
		GetClientRect(_mHwnd, &rect);
		MapWindowPoints(_mHwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}

	void Window::FreeCursor() noexcept
	{
		ClipCursor(nullptr);
	}

	void Window::ShowCursor() noexcept
	{
		while (::ShowCursor(TRUE) < 0);
	}

	void Window::HideCursor() noexcept
	{
		while (::ShowCursor(FALSE) >= 0);
	}

	void Window::EnableImGuiMouse()noexcept
	{
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}

	void Window::DisableImGuiMouse()noexcept
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
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
		
		
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lparam))
		{
			return true;
		}
		
		const auto& imIo = ImGui::GetIO();
		switch (msg)
		{
			case WM_CLOSE:
			{
				PostQuitMessage(0);
				return 0;
			}
			case WM_KILLFOCUS:
			{
				_mKey.ClearState();
				break;
			}
			case WM_ACTIVATE:
				//confine/free cursor on window to foreground/background if cursor disabled
				if (!_mCursorEnabled)
				{
					if (wParam & WA_ACTIVE)
					{
						ConfineCursor();
						HideCursor();
					}
					else
					{
						FreeCursor();
						ShowCursor();
					}
				}
				break;
				/********* KEYBOARD *********/
			case WM_KEYDOWN:
				// syskey commands need to be handled to track ALT key (VK_MENU) and F10
			case WM_SYSKEYDOWN:
			{
				if (imIo.WantCaptureKeyboard)
				{
					break;
				}
				// stifle this keyboard message if imgui wants to capture
				if (!(lparam & 0x40000000) || _mKey.AutorepeatIsEnabled())
				{
					_mKey.OnKeyPressed(static_cast<unsigned char>(wParam));
				}
				break;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				if (imIo.WantCaptureKeyboard)
				{
					break;
				}
				_mKey.OnKeyReleased(static_cast<unsigned char>(wParam));
				break;
			}
			case WM_CHAR:
				if (imIo.WantCaptureKeyboard)
				{
					break;
				}
				_mKey.OnChar(static_cast<unsigned char>(wParam));
				break;
				//############### MOUSE MESSAGES ########################################
			case WM_MOUSEMOVE:
			{
				const POINTS pt = MAKEPOINTS(lparam);
				if (!_mCursorEnabled)
				{
					if (!_mMouse.IsInWindow())
					{
						SetCapture(hwnd);
						_mMouse.OnMouseEnter();
						HideCursor();
					}
					break;
				}
				if (imIo.WantCaptureMouse)
				{
					break;
				}
				//cursorless exclusive gets first dibs
				if (pt.x >= 0 && pt.x < _mWidth && pt.y >= 0 && pt.y < _mHeight)
				{
					_mMouse.OnMouseMove(pt.x, pt.y);
					if (!_mMouse.IsInWindow())
					{
						SetCapture(hwnd);
						_mMouse.OnMouseEnter();
					}
				}
				//not in client -> log move /maintain capture if button down
				else
				{
					if (wParam & (MK_LBUTTON | MK_RBUTTON))
					{
						_mMouse.OnMouseMove(pt.x, pt.y);
					}
					else
					{
						ReleaseCapture();
						_mMouse.OnMouseLeave();
					}
				}
				break;
			}
			case WM_LBUTTONDOWN:
			{
				SetForegroundWindow(hwnd);
				if (!_mCursorEnabled)
				{
					ConfineCursor();
					HideCursor();
				}
				if (imIo.WantCaptureMouse)
				{
					break;
				}
				const POINTS pt = MAKEPOINTS(lparam);
				_mMouse.OnLeftPressed(pt.x, pt.y);
				break;
			}
			case WM_RBUTTONDOWN:
			{
				//stifle this mouse message if imgui want to capture
				if (imIo.WantCaptureMouse)
				{
					break;
				}
				const POINTS pt = MAKEPOINTS(lparam);
				_mMouse.OnRightPressed(pt.x, pt.y);
				break;
			}
			case WM_LBUTTONUP:
			{
				//stifle this mouse message if imgui want to capture
				if (imIo.WantCaptureMouse)
				{
					break;
				}
				const POINTS pt = MAKEPOINTS(lparam);
				_mMouse.OnLeftReleased(pt.x, pt.y);
				if (pt.x < 0 || pt.x >= _mWidth || pt.y < 0 || pt.y >= _mHeight)
				{
					ReleaseCapture();
					_mMouse.OnMouseLeave();
				}
				break;
			}
			case WM_RBUTTONUP:
			{
				//stifle this mouse message if imgui want to capture
				if (imIo.WantCaptureMouse)
				{
					break;
				}
				const POINTS pt = MAKEPOINTS(lparam);
				_mMouse.OnRightReleased(pt.x, pt.y);
				if (pt.x < 0 || pt.x >= _mWidth || pt.y < 0 || pt.y >= _mHeight)
				{
					ReleaseCapture();
					_mMouse.OnMouseLeave();
				}
				break;
			}
			case WM_MOUSEWHEEL:
			{

				//stifle this mouse message if imgui want to capture
				if (imIo.WantCaptureMouse)
				{
					break;
				}
				const POINTS pt = MAKEPOINTS(lparam);
				const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
				_mMouse.OnWheelDelta(pt.x, pt.y, delta);
				break;
			}
			// ########### RAW MOUSE MESSAGES ######################//
			case WM_INPUT:
			{
				if (!_mMouse.RawEnabled())
				{
					break;
				}
				UINT size = 0;
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
				{
					break;
				}
				_mRawBuffer.resize(size);
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, _mRawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
				{
					break;
				}
				auto& ri = reinterpret_cast<const RAWINPUT&>(*_mRawBuffer.data());
				if (ri.header.dwType == RIM_TYPEMOUSE &&
					(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
				{
					_mMouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
				}
				break;
			}
		}
		return DefWindowProc(hwnd, msg, wParam, lparam);
	}
}