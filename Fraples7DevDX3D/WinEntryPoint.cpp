#include "Source/Fraples7DevDX3D/Core/Fraples7.h"
#include "Source/Fraples7DevDX3D/Platform/Windows/Window.h"
#include "Source/Fraples7DevDX3D/Core/Exceptions/WindowsThrowMacros.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	FraplesDev::Window win = FraplesDev::Window(320, 720, "Ban Max");
	MSG msg;
	BOOL gResult;
	while ((gResult =GetMessage(&msg,nullptr,0,0))>0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (win._mKey.KeyIsPressed(VK_SPACE))
		{
			MessageBox(nullptr, "Fraples Seven Best Engine Ever!", nullptr, MB_OK | MB_ICONEXCLAMATION);
		}
	}
	if (gResult == -1)
	{
		throw FPL_LAST_EXCEPT();
	}
	return msg.wParam;
}