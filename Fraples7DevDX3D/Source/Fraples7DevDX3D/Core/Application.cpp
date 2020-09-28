#include "Application.h"
#include "Debugging/Exceptions/Macros/WindowsThrowMacros.h"
#include "Debugging/Timer.h"
#include "../SandBox/Box.h"

namespace FraplesDev {
	Application::Application(const char* name, int width, int height)
		:_mWin(name, width, height)
	{

		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float>adist(0.0f, 3.1415 * 2.0f);
		std::uniform_real_distribution<float>ddist(0.0f, 3.1415 * 2.0f);
		std::uniform_real_distribution<float>odist(0.0f, 3.1415 * 0.3f);
		std::uniform_real_distribution<float>rdist(6.0f, 20.0f);

		for (size_t i = 0; i < 1+ std::rand() % 80; i++)
		{
			_mBoxes.push_back(std::make_unique<Box>(_mWin.GetGFX(), rng, adist, ddist, odist, rdist));
		}
		_mWin.GetGFX().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	}
	Application::Application():_mWin("Fraples7 Engine Studio", 1920,1080)
	{

	}

	Application::~Application()
	{
	}

	int Application::StartApp()
	{
		// process all messages pending, but to not block for new messages
		while (true)
		{
			if (const auto& ecode = FraplesDev::Window::ProcessMessages())
			{// if return optional has value, means we're quitting so return exit code
				return *ecode;
			}
			// execute the game logic
			// const auto timer = _mTimer.Start() * _mSpeedFactor;
			DoFrame();
		}
	}

	void Application::DoFrame()
	{
		auto dt = _mTimer.Time();
		_mWin.GetGFX().ClearBuffer(0.07f, 0.0f, 0.12f);
		for (auto& b : _mBoxes)
		{
			b->Update(dt);
			b->Render(_mWin.GetGFX());
		}
		_mWin.GetGFX().EndFrame();
	}
}