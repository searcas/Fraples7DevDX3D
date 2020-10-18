#include "ImGuiMgr.h"
#include "imgui.h"

namespace FraplesDev
{

	ImGuiMgr::ImGuiMgr()
	{
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	ImGuiMgr::~ImGuiMgr()
	{
		ImGui::DestroyContext();
	}
}