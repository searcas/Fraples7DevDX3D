#include "ImGuiMgr.h"
#include "imgui.h"
#include <filesystem>
namespace FraplesDev
{

	ImGuiMgr::ImGuiMgr()
	{
		if (!std::filesystem::exists("imgui.ini") && std::filesystem::exists("imgui_default.ini"))
		{
			std::filesystem::copy_file("imgui_default.ini", "imgui.ini");
		}
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	ImGuiMgr::~ImGuiMgr()
	{
		ImGui::DestroyContext();
	}
}