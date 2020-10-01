#include "Source/Fraples7DevDX3D/Core/Fraples7.h"
#include "Source/Fraples7DevDX3D/Core/Application.h"
#include "Source/Fraples7DevDX3D/Core/Debugging/Exceptions/FraplesException.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		return FraplesDev::Application("Fraples7 Engine Studio ", 800,600).StartApp();
	}
	catch (const FraplesDev::FraplesException & e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch(const std::exception& e)
	{
		MessageBox(nullptr, e.what(),  "Standard Exception", MB_OK | MB_ICONEXCLAMATION);

	}
	catch (...)
	{
		MessageBox(nullptr, "No details available","Unknown Excaption", MB_OK | MB_ICONEXCLAMATION);
	}
	

	return 0;
}