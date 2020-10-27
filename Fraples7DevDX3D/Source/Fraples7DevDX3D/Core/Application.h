#pragma once

#include "../Platform/Windows/Window.h"
#include "../Core/Debugging/Timer.h"
#include "../RendererAPI/Camera.h"
#include "../Objects/PointLight.h"
#include "RendererAPI/Mesh/Model.h"
#include <set>
#include "Math/Math.h"
#include "Objects/Box.h"
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
		void ShowImguiDemoWindow();
		void SpawnAppInfoWindow() noexcept;
		//	Not in use anymore
		//	void SpawnBoxWindowManagerWindow() noexcept;
		//	void SpawnBoxWindows() noexcept;
		void ShowRawInputWindow()noexcept;
	private:
		FraplesDev::Window _mWin;
	private:
		//mouse
		int _mX = 0, _mY = 0;

	private:
		//objects
		Camera _mCamera;
		PointLight light;
		Model _mNano{ _mWin.GetGFX(),"Models\\nano_textured\\nanosuit.obj" };
	private:
		FraplesDev::Timer _mTimer;
		float _mSpeedFactor = 1.0f;
		bool show_demo_window = true;
		float speed_accelerator = 1.500f;
	
	};

}