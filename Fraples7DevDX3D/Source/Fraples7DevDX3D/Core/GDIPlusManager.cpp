#define WIN_TAR
#include "GDIPlusManager.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
namespace FraplesDev
{
	ULONG_PTR GDIPlusManager::s_token = 0;
	int GDIPlusManager::s_refCount = 0;
	GDIPlusManager::GDIPlusManager()
	{
		if (s_refCount++ == 0)
		{
			Gdiplus::GdiplusStartupInput input;
			input.GdiplusVersion = 1;
			input.DebugEventCallback = nullptr;
			input.SuppressBackgroundThread = false;
			Gdiplus::GdiplusStartup(&s_token, &input, nullptr);
		}
	}

	GDIPlusManager::~GDIPlusManager()
	{
		if (--s_refCount == 0)
		{
			Gdiplus::GdiplusShutdown(s_token);
		}
	}
}