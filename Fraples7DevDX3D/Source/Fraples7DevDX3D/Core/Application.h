#pragma once
#include "Platform/Windows/Window.h"
#include "Core/Debugging/Timer.h"
#include "RendererAPI/Camera.h"
#include "RendererAPI/Mesh/Model.h"
#include "Math/Math.h"
#include "Objects/BasePlane.h"
#include "Objects/BaseCube.h"
#include "Objects/PointLight.h"

#include <set>
namespace FraplesDev
{


	class Application
	{
	public:
		Application(const char* name, int width, int height);
		~Application();
		int StartApp();
	private:
		void SpawnFunc();
		void RenderObj();
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
		PointLight light;
		Camera _mCamera;
		BasePlane plane;
		BaseCube cube;
		//Model _mNano{ _mWin.GetGFX(),"Models\\nano_textured\\nanosuit.obj" };
		Model _mWall{ _mWin.GetGFX(),"Models\\brick_wall\\brick_wall.obj" };

	private:
		FraplesDev::Timer _mTimer;
		float _mSpeedFactor = 1.0f;
		bool show_demo_window = true;
		float speed_accelerator = 1.500f;
	
	};

}