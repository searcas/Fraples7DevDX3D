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
		Application(const char* name, int width, int height, const std::string& commandLine);
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
		Model _mSponza{ _mWin.GetGFX(),"Models\\Sponza\\sponza.obj",0.1f};
	private:
		FraplesDev::Timer _mTimer;
		float _mSpeedFactor = 1.0f;
		bool show_demo_window = true;
		float speed_accelerator = 1.500f;
		std::string _mCommandLine;
	
	};

}