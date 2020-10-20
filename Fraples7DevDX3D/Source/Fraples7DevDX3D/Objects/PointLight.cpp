#include "PointLight.h"

FraplesDev::PointLight::PointLight(Graphics& gfx, float radius) :
	_mMesh(gfx, radius), _mCbuf(gfx)
{

}

void FraplesDev::PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position of Light");
		ImGui::SliderFloat("X", &pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -60.0f, 60.0f, "%.1f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void FraplesDev::PointLight::Reset() noexcept
{
	pos = { 0.0f, 0.0f, 0.0f};
}

void FraplesDev::PointLight::Render(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	_mMesh.SetPos(pos);
	_mMesh.Render(gfx);
}

void FraplesDev::PointLight::Bind(Graphics& gfx) const noexcept
{
	_mCbuf.Update(gfx, PointLightCBuf{ pos });
}
