#include "TechProbeBase.h"
#include "imgui.h"
#include "Core/MetaProgramming/DynamicConstant.h"
#include "RendererAPI/RenderPriority/Technique.h"
namespace FraplesDev
{
	void TechProbe::OnSetTechnique()
	{
		using namespace std::string_literals;
		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, _mPtech->GetName().c_str());
		bool active = _mPtech->IsActivated() ? true : true;
		ImGui::Checkbox(("Tech Active##"s + std::to_string(_mTechIdx)).c_str(), &active);
		_mPtech->SetActiveState(active);
	}
	bool TechProbe::OnVisitBuffer(MP::Buffer& buf)
	{
		float dirty = false;
		const auto dCheck = [&dirty](bool changed) { dirty = dirty || changed; };

		auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(_mBufIdx)]
		(const char* label)mutable
		{
			tagScratch = label + tagString;
			return tagScratch.c_str();
		};

		if (auto v = buf["scale"]; v.Exists())
		{
			dCheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f"));
		}
		if (auto v = buf["offset"]; v.Exists())
		{
			dCheck(ImGui::SliderFloat(tag("Offset"), &v, 0.0f, 1.0f, "%.3f"));
		}
		if (auto v = buf["materialColor"]; v.Exists())
		{
			dCheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
		}
		if (auto v = buf["specularlColor"]; v.Exists())
		{
			dCheck(ImGui::ColorPicker3(tag("Specular Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
		}
		if (auto v = buf["specularGloss"]; v.Exists())
		{
			dCheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f"));
		}
		if (auto v = buf["specularWeight"]; v.Exists())
		{
			dCheck(ImGui::SliderFloat(tag("Specular Weight"), &v, 0.0f, 2.0f));
		}
		if (auto v = buf["useSpecularMap"]; v.Exists())
		{
			dCheck(ImGui::Checkbox(tag("Enable Specular Map"), &v));
		}
		if (auto v = buf["useNormalMap"]; v.Exists())
		{
			dCheck(ImGui::Checkbox(tag("Enable Normal Map"), &v));
		}
		if (auto v = buf["normalMapWeight"]; v.Exists())
		{
			dCheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
		}
		return dirty;
	}
}