#pragma once

#include "../Platform/Windows/Window.h"
#include "../Core/Debugging/Timer.h"
#include "../Objects/Box.h"
namespace FraplesDev
{


	class Application
	{
	public:
		Application(const char* name, int width, int height);
		Application();
		~Application();
		int StartApp();
	private:
		void DoFrame();
	private:
		FraplesDev::Window _mWin;
		FraplesDev::Timer _mTimer;
		float _mSpeedFactor = 1.0f;
		std::vector<std::unique_ptr<class Renderer>>_mrenderable;
		static constexpr size_t nDrawables = 180;
	};

}