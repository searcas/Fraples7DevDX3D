#include "Application.h"
#include "ImGui/imgui.h"
#include "QualityAssurance/TestingQA.h"
#include "RendererAPI/Probe/ModelProbeBase.h"
#include "RendererAPI/Camera/Camera.h"
namespace FraplesDev
{
	
	Application::Application(const char* name, int width, int height, const std::string& commandLine)
		:_mWin(name, width, height), light(_mWin.GetGFX()), scriptCommander(Utility::TokenizeQuoted(commandLine))
	{
		//QA::TestMaterialSystemLoading(_mWin.GetGFX());
		//QA::TestDynamicConstant();
		//QA::D3DTestScratchPad(_mWin);
		_mCameras.AddCamera(std::make_unique<Camera>(_mWin.GetGFX(),"A", DirectX::XMFLOAT3{ -13.5,6.0f,3.5f }, 0.0f, PI / 2.0f));
		_mCameras.AddCamera(std::make_unique<Camera>(_mWin.GetGFX(),"B", DirectX::XMFLOAT3{ -13.5,28.8f,-6.4f },PI / 180.0f * 13.0f,PI / 180 * 61.0f));
		cube1.SetPos({ 4.0f,0.0f,0.0f });
		cube2.SetPos({ 0.0f,4.0f,0.0f });
		_mNano.SetRootTransform( DirectX::XMMatrixRotationY(PI / 2.0f) *
			DirectX::XMMatrixTranslation(27.f,-0.56f,1.7f) );
		gobber.SetRootTransform(DirectX::XMMatrixRotationY(-PI / 2.0f) *
			DirectX::XMMatrixTranslation(-8.0f, 10.0f, 0.f));
		cube1.LinkTechniques(renderGraph);
		cube2.LinkTechniques(renderGraph);
		_mSponza.LinkTechniques(renderGraph);
		light.LinkTechniques(renderGraph);

		gobber.LinkTechniques(renderGraph);
		_mNano.LinkTechniques(renderGraph);
		_mCameras.LinnkTechniques(renderGraph);

		//bluePlane.SetPosXYZ(_mCameras.GetCamera().GetPos());
		//redPlane.SetPosXYZ(_mCameras.GetCamera().GetPos());
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
		_mCameras.SpawnWindow();
		
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
		_mCameras.Submit();
		light.Submit();
		_mSponza.Submit();
		cube1.Submit();
		cube2.Submit();
		_mNano.Submit();
		gobber.Submit();
		renderGraph.Execute(_mWin.GetGFX());
	}

	void Application::DoFrame()
	{
		const auto dt = _mTimer.Get() * speed_accelerator;
		_mWin.GetGFX().BeginFrame(0.000069f, 0.000069f, 0.06f);
		//_mWin.GetGFX().BeginFrame(0.19f, 0.19f, 0.19f);
		_mCameras.GetCamera().BindGraphics(_mWin.GetGFX());
		light.Bind(_mWin.GetGFX(), _mCameras.GetCamera().GetMatrix());
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
				_mCameras.GetCamera().Translate({ 0.0f, 0.0f, dt });
			}
			if (_mWin._mKey.KeyIsPressed('S'))
			{
				_mCameras.GetCamera().Translate({ 0.0f, 0.0f, -dt });
			}
			if (_mWin._mKey.KeyIsPressed('A'))
			{
				_mCameras.GetCamera().Translate({ -dt, 0.0f, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed('D'))
			{
				_mCameras.GetCamera().Translate({ dt, 0.0f, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed(VK_SPACE))
			{
				_mCameras.GetCamera().Translate({ 0.0f, dt, 0.0f });
			}
			if (_mWin._mKey.KeyIsPressed(VK_CONTROL))
			{
				_mCameras.GetCamera().Translate({ 0.0f, -dt, 0.0f });
			}

		}

		while (const auto delta = _mWin._mMouse.ReadRawDelta())
		{
			if (!_mWin.IsCursorEnabled())
			{
				_mCameras.GetCamera().Rotate((float)delta->x, (float)delta->y);
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

