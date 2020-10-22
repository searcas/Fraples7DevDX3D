#include "Application.h"
#include "Debugging/Exceptions/Macros/WindowsThrowMacros.h"
#include "Debugging/Timer.h"
// ########## Objects ################
#include "../Objects/Box.h"
#include "../Objects/Cylinder.h"
//#include "../Objects/SkinnedBox.h"
//#include "../Objects/Sphere.h"
//#include "../Objects/Pyramid.h"
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
namespace FraplesDev
{
	GDIPlusManager gdipm;

	Application::Application(const char* name, int width, int height)
		:_mWin(name, width, height),light(_mWin.GetGFX())
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
	

				const DirectX::XMFLOAT3 mat = { cdist(rng),cdist(rng),cdist(rng) };
				switch (sdist(rng))
				{
				case 0:
					return std::make_unique<Box>(
						_mGfx, rng, adist, ddist,
						odist, rdist, bdist, mat
						);
				case 1:
					return std::make_unique<Cylinder>(_mGfx, rng, adist, ddist, odist, rdist, bdist, tdist);

				default:
					assert(false && "Impossible drawable option in factory");
					return { };
				}
				
			
			}
		private:
			Graphics& _mGfx;
			std::mt19937 rng{ std::random_device{}() };
			std::uniform_int_distribution<int> sdist{ 0,1};
			std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
			std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
			std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
			std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
			std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
			std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
			std::uniform_int_distribution<int> latdist{ 5, 20 };
			std::uniform_int_distribution<int> longdist{ 10, 50 };
			std::uniform_int_distribution<int> typedist{ 0, 4 };
			std::uniform_int_distribution<int> tdist{ 3,30 };

		};
		_mrenderable.reserve(nDrawables);
		std::generate_n(std::back_inserter(_mrenderable), nDrawables, Factory{ _mWin.GetGFX() });

		_mWin.GetGFX().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
		_mWin.GetGFX().SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
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
		_mWin.GetGFX().SetCamera(_mCamera.GetMatrix());
		light.Bind(_mWin.GetGFX(),_mCamera.GetMatrix());
		//if (_mWin._mKey.KeyIsPressed(VK_SPACE))
		//{
		//	_mWin.GetGFX().DisableImGui();
		//}
		//else
		//{
		//	_mWin.GetGFX().EnableImGui();
		//}
		
		_mWin.GetGFX().BeginFrame(0.0f, 0.017f, 0.021f);

		for (auto& b : _mrenderable)
		{
			b->Update(_mWin._mKey.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
			b->Render(_mWin.GetGFX());
		}
		light.Render(_mWin.GetGFX());
		static char buffer[1024];

		if (ImGui::Begin("Speed Controller"))
		{
			ImGui::SliderFloat("Speed Factor", &speed_accelerator, 0.0f, 6.0f,"%.4f",ImGuiSliderFlags_Logarithmic);
			ImGui::Text("Application Average %.3f ms/frame (%.1f Fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::InputText("Commands: ", buffer,1024);

		}
		ImGui::End();

		_mCamera.SpawnControllWindow();
		light.SpawnControlWindow();
		_mWin.GetGFX().EndFrame();

	}

}
