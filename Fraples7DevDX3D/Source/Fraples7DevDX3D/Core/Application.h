#pragma once

#include "../Platform/Windows/Window.h"
#include "../Core/Debugging/Timer.h"
#include "../RendererAPI/Camera.h"
#include "../Objects/PointLight.h"
#include <set>

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
		void SpawnSimulationWindow() noexcept;
		void SpawnBoxWindowManagerWindow() noexcept;
		void SpawnBoxWindows() noexcept;
	private:
		std::vector<class Box*>boxes;
		FraplesDev::Window _mWin;
		FraplesDev::Timer _mTimer;
		float _mSpeedFactor = 1.0f;
		std::vector<std::unique_ptr<class Renderer>>_mrenderable;
		static constexpr size_t nDrawables = 180;
		bool show_demo_window = true;
		float speed_accelerator = 1.500f;
		Camera _mCamera;
		PointLight light;
		std::optional<int>comboBoxIndex;
		std::set<int> boxControlIds;
	};

}