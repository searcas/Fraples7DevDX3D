#include "PointLight.h"

FraplesDev::PointLight::PointLight(Graphics& gfx, float radius) :
	_mMesh(gfx, radius), _mCbuf(gfx)
{
	Reset();
}

void FraplesDev::PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position of Light");
		ImGui::SliderFloat("X", &_mConstantBufferData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &_mConstantBufferData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &_mConstantBufferData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity-Color");
		ImGui::SliderFloat("Intensity", &_mConstantBufferData.diffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit3("Diffuse Color", &_mConstantBufferData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient Color", &_mConstantBufferData.ambient.x);
	

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &_mConstantBufferData.attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Linear", &_mConstantBufferData.attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Quadratic", &_mConstantBufferData.attQuad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void FraplesDev::PointLight::Reset() noexcept
{
	_mConstantBufferData = {
		{0.0f,0.0f,0.0f,},
		{0.05f,0.05f,0.05f,},
		{1.0f,1.0f,1.0f,},
		1.0f,
		1.0f,
		0.045f,
		0.0075
	};
}

void FraplesDev::PointLight::Render(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	_mMesh.SetPos(_mConstantBufferData.pos);
	_mMesh.Render(gfx);
}

void FraplesDev::PointLight::Bind(Graphics& gfx) const noexcept
{
	_mCbuf.Update(gfx, _mConstantBufferData);
	_mCbuf.Bind(gfx);
}