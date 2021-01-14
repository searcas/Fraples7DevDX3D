#pragma once
#include "Platform/Windows/Window.h"
#include "Utility/ScriptCommandCenter.h"
#include "Objects/PointLight.h"
#include "RendererAPI/Camera.h"
#include "Objects/BaseCube.h"
#include "RendererAPI/Mesh/Model.h"
#include "Utility/Timer.h"
#include "RendererAPI/RenderGraph/ScaleOutlineRenderGraph.h"
#include "RendererAPI/RenderGraph/BlurOutlineRenderGraph.h"
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
		void HandleEvents(float dt);
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
		ScriptCommander scriptCommander;
		BlurOutlineRenderGraph renderGraph{ _mWin.GetGFX() };
		//objects
		PointLight light;
		Camera _mCamera;
		//std::unique_ptr<Mesh>_mPloaded;
		BaseCube cube1 = { _mWin.GetGFX(),4.0f };
		BaseCube cube2 = { _mWin.GetGFX(),4.0f };
		Model gobber{ _mWin.GetGFX(),"Models\\gobber\\GoblinX.obj", 4.0f};
		Model _mSponza{ _mWin.GetGFX(),"Models\\Sponza\\sponza.obj", 1.0f / 20.0f };
		Model _mNano{ _mWin.GetGFX(),"Models\\nano_textured\\nanosuit.obj", 2.0f };
		//	BasePlane bluePlane{ _mWin.GetGFX(),6.0f,{0.3,0.3,1.0f,0.0f} };
		//	BasePlane redPlane{ _mWin.GetGFX(),6.0f,{1.0,0.3,0.4f,0.0f} };
		
	private:
		FraplesDev::Timer _mTimer;
		float _mSpeedFactor = 1.0f;
		bool show_demo_window = true;
		float speed_accelerator = 1.500f;
	};

}