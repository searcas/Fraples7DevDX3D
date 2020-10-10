#include "Application.h"
#include "Debugging/Exceptions/Macros/WindowsThrowMacros.h"
#include "Debugging/Timer.h"
#include "../SandBox/Box.h"
#include "../SandBox/Sphere.h"
#include "../SandBox/Pyramid.h"
#include "../SandBox/Melon.h"
#include "Math/Math.h"
#include <algorithm>

namespace FraplesDev
{
	Application::Application(const char* name, int width, int height)
		:_mWin(name, width, height)
	{
		class Factory
		{
		public:
			Factory(Graphics& gfx) :
				_mGfx(gfx)
			{

			}
			std::unique_ptr<Renderer> operator()()
			{
				switch (typedist(rng))
				{
				case 0:
					return std::make_unique<Pyramid>(
						_mGfx, rng, adist, ddist,
						odist, rdist);
				case 1:
					return std::make_unique<Box>(
						_mGfx, rng, adist, ddist,
						odist, rdist, bdist
						);
				case 2:
					return std::make_unique<Melon>(
						_mGfx, rng, adist, ddist,
						odist, rdist, longdist, latdist
						);
				default:
					assert(false && "bad drawable type in factory");
					return {};
				}
			}
		private:
			Graphics& _mGfx;
			std::mt19937 rng{ std::random_device{}() };
			std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
			std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
			std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
			std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
			std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
			std::uniform_int_distribution<int> latdist{ 5, 20 };
			std::uniform_int_distribution<int> longdist{ 10, 50 };
			std::uniform_int_distribution<int> typedist{ 0, 2 };
		};

		Factory f(_mWin.GetGFX());
		_mrenderable.reserve(nDrawables);
		std::generate_n(std::back_inserter(_mrenderable), nDrawables, f);


		_mWin.GetGFX().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	}


		

	Application::Application()
		: _mWin("Fraples7 Engine Studio", 1920, 1080)
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
		const auto dt = _mTimer.Get();
		_mWin.GetGFX().ClearBuffer(0.87f, 0.017f, 0.021f);
		for (auto& b : _mrenderable)
		{
			b->Update(dt);
			b->Render(_mWin.GetGFX());
		}
		_mWin.GetGFX().EndFrame();
	}

}
