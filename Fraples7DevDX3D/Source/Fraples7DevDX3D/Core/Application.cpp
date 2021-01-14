#include "Application.h"
#include "ImGui/imgui.h"
#include "QualityAssurance/TestingQA.h"

#include "RendererAPI/Probe/ModelProbeBase.h"
namespace FraplesDev
{
	
	Application::Application(const char* name, int width, int height, const std::string& commandLine)
		:_mWin(name, width, height), light(_mWin.GetGFX()), scriptCommander(Utility::TokenizeQuoted(commandLine))
	{
		//QA::TestMaterialSystemLoading(_mWin.GetGFX());
		QA::TestDynamicConstant();
		cube1.SetPos({ 4.0f,0.0f,0.0f });
		cube2.SetPos({ 0.0f,4.0f,0.0f });
		cube1.LinkTechniques(renderGraph);
		cube2.LinkTechniques(renderGraph);
		_mSponza.LinkTechniques(renderGraph);
		light.LinkTechniques(renderGraph);
		gobber.LinkTechniques(renderGraph);
		_mNano.LinkTechniques(renderGraph);
		//bluePlane.SetPosXYZ(_mCamera.GetPos());
		//redPlane.SetPosXYZ(_mCamera.GetPos());
		_mWin.GetGFX().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 800.0f));
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
			{
				// if return optional has value, means we're quitting so return exit code
				return *ecode;
			}
			// execute the game logic
			 const auto dt = _mTimer.Get() * _mSpeedFactor;
			HandleEvents(dt);
			DoFrame();
		}
	}

	void Application::SpawnFunc()
	{
		_mCamera.SpawnControllWindow();
		light.SpawnControlWindow();
		SpawnAppInfoWindow();
		ShowRawInputWindow();
		//bluePlane.SpawnControlWindow(_mWin.GetGFX(),"BLUE ONE");
		//redPlane.SpawnControlWindow(_mWin.GetGFX(), "RED ONE");
		cube1.SpawnControlWindow(_mWin.GetGFX(), "Cube 1");
		cube2.SpawnControlWindow(_mWin.GetGFX(), "Cube 2");
		renderGraph.RenderWidgets(_mWin.GetGFX());
	}

	void Application::RenderObj()
	{
		light.Submit();
		_mSponza.Submit();
		cube1.Submit();
		cube2.Submit();
	//	gobber.Submit(fc);
		renderGraph.Execute(_mWin.GetGFX());
	}

	void Application::DoFrame()
	{
		const auto dt = _mTimer.Get() * speed_accelerator;
		_mWin.GetGFX().BeginFrame(0.000069f, 0.000069f, 0.06f);
		//_mWin.GetGFX().BeginFrame(0.19f, 0.19f, 0.19f);
		_mWin.GetGFX().SetCamera(_mCamera.GetMatrix());
		light.Bind(_mWin.GetGFX(), _mCamera.GetMatrix());
		RenderObj();

	
		static ModelProbeBase sponza{ "Sponza" };
		static ModelProbeBase nan{ "Nano" };
		static ModelProbeBase gob("Gobber");
		sponza.SpawnWindow(_mSponza);
		nan.SpawnWindow(_mNano);
		gob.SpawnWindow(gobber);
		SpawnFunc();
		_mWin.GetGFX().EndFrame();
		renderGraph.Reset();
	}

	void Application::HandleEvents(float dt)
	{
		while (const auto e = _mWin._mKey.Readkey())
		{
			if (!e->isPressed())
				continue;

			switch (e->GetCode())
			{
			case VK_ESCAPE:
				if (_mWin.IsCursorEnabled())
				{
					_mWin.DisableCursor();
					_mWin._mMouse.EnableRaw();
				}
				else
				{
					_mWin.EnableCursor();
					_mWin._mMouse.DisableRaw();
				}
				break;
			case VK_F1:
				show_demo_window = true;
				break;
			}
		}
		if (!_mWin.IsCursorEnabled())
		{
			if (_mWin._mKey.KeyIsPressed('W'))
			{
				_mCamera.Translate({ 0.0f, 0.0f, dt });
			}
			if (_mWin._mKey.KeyIsPressed('S'))
			{
				_mCamera.Translate({ 0.0f, 0.0f, -dt });
			}
			if (_mWin._mKey.KeyIsPressed('A'))
			{
				_mCamera.Translate({ -dt, 0.0f, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed('D'))
			{
				_mCamera.Translate({ dt, 0.0f, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed(VK_SPACE))
			{
				_mCamera.Translate({ 0.0f, dt, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed(VK_CONTROL))
			{
				_mCamera.Translate({ 0.0f, -dt, 0.0f });
			}

		}

		while (const auto delta = _mWin._mMouse.ReadRawDelta())
		{
			if (!_mWin.IsCursorEnabled())
			{
				_mCamera.Rotate((float)delta->x, (float)delta->y);
			}

		}
	}


	void Application::ShowImguiDemoWindow()
	{
		static bool show_demo_window = true;
		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(&show_demo_window);
		}
	}

	void Application::SpawnAppInfoWindow()noexcept
	{
		if (ImGui::Begin("Perfomance"))
		{
			ImGui::BulletText("Application Average %.3f ms/frame (%.1f Fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}
	void Application::ShowRawInputWindow() noexcept
	{
		
		if (ImGui::Begin("Raw Input"))
		{
			ImGui::Text("(X, Y): (%d, %d)", _mWin._mMouse.GetPosX(), _mWin._mMouse.GetPosY());
			ImGui::Text("Cursor: %s", _mWin.IsCursorEnabled() ? "Enabled" : "Disabled");
		}
		ImGui::End();
	}
	
}

