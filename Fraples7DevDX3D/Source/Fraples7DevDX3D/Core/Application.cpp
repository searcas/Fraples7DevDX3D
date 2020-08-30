#include "Application.h"
#include "Debugging/Exceptions/WindowsThrowMacros.h"
#include "Debugging/Timer.h"


Application::Application(const char* name,int width, int height) 
{
	_mWin = new FraplesDev::Window(width, height, name);
}

Application::Application()
{
	_mWin = new FraplesDev::Window(720, 1080, "Fraples7 ENGINE");
}

Application::~Application()
{
	delete _mWin;
}

int Application::StartApp()
{
	// process all messages pending, but to not block for new messages
	while (true)
	{
		if (const auto& ecode = FraplesDev::Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		// execute the game logic
		const auto timer = _mTimer.Start() * _mSpeedFactor;
		DoFrame();
	}
}

void Application::DoFrame()
{
	const float c = sin(_mTimer.Time()) / 2.0f + 0.5f;
	_mWin->GetGFX().ClearBuffer(c, c, 1.0f);
	_mWin->GetGFX().EndFrame();
}
