#include "Application.h"
#include "Debugging/Exceptions/Macros/WindowsThrowMacros.h"
#include "Debugging/Timer.h"
// ########## Objects ################
#include "../Objects/Box.h"
#include "../Objects/Cylinder.h"
#include "../Objects/Pyramid.h"
#include "../Objects/SkinnedBox.h"
#include "../Objects/AssetTest.h"
//#include "../Objects/Sphere.h"
//#include "../Objects/Melon.h"
//#include "../Objects/Sheet.h"
//########### End Objects ###########
#include "Math/Math.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <algorithm>
#include "Core/MetaProgramming/Vertex.h"
namespace FraplesDev
{
	GDIPlusManager gdipm;


	Application::Application(const char* name, int width, int height)
		:_mWin(name, width, height), light(_mWin.GetGFX())
	{
	
		_mWin.GetGFX().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	}


	Application::Application()
		: _mWin("Fraples7 Engine Studio", 1920, 1080),light(_mWin.GetGFX())
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
		const auto dt = _mTimer.Get() * speed_accelerator;
		_mWin.GetGFX().BeginFrame(0.07f, 0.017f, 0.021f);
		_mWin.GetGFX().SetCamera(_mCamera.GetMatrix());
		light.Bind(_mWin.GetGFX(), _mCamera.GetMatrix());
		
		_mNano.Render(_mWin.GetGFX());

		light.Render(_mWin.GetGFX());

		_mCamera.SpawnControllWindow();
		light.SpawnControlWindow();
		_mWin.GetGFX().EndFrame();
	}

	void Application::SpawnSimulationWindow()noexcept
	{
		static char buffer[1024];

		if (ImGui::Begin("Speed Controller"))
		{
			ImGui::SliderFloat("Speed Factor", &speed_accelerator, 0.0f, 6.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
			ImGui::Text("Application Average %.3f ms/frame (%.1f Fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::InputText("Commands: ", buffer, 1024);

		}
		ImGui::End();
	}
	void Application::SpawnBoxWindowManagerWindow()noexcept
	{

		if (ImGui::Begin("Boxes"))
		{
			using namespace std::string_literals;
			const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Chose a box..."s;
			if (ImGui::BeginCombo("Box Number ", preview.c_str()))
			{
				for (int i = 0; i < boxes.size(); i++)
				{
					const bool selected = *comboBoxIndex == i;
					if (ImGui::Selectable(std::to_string(i).c_str(), selected))
					{
						comboBoxIndex = i;
					}
					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
			{
				boxControlIds.insert(*comboBoxIndex);
				comboBoxIndex.reset();
			}
		}
		ImGui::End();
	}
	
	//imgui box attribute control windows
	void Application::SpawnBoxWindows() noexcept
	{
		for (auto i = boxControlIds.begin(); i!= boxControlIds.end();)
		{
			if (!boxes[*i]->SpawnControlWindow(*i,_mWin.GetGFX()))
			{
				i = boxControlIds.erase(i);
			}
			else
			{
				i++;
			}
		}
		_mWin.GetGFX().EndFrame();
	}
	}

