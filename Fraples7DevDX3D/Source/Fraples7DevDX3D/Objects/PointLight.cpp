#include "PointLight.h"
#include "imgui.h"
namespace FraplesDev
{
	PointLight::PointLight(Graphics& gfx, float radius) :
		_mMesh(gfx, radius), _mCbuf(gfx)
	{
		Reset();
	}

	void PointLight::SpawnControlWindow() noexcept
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
	void PointLight::LinkTechniques(RenderGraph& rg)
	{
		_mMesh.LinkTechniques(rg);
	}
	void PointLight::Reset() noexcept
	{
		_mConstantBufferData = {
			{10.0f,9.0f,-2.5f},
			{0.05f,0.05f,0.05f,},
			{1.0f,1.0f,1.0f,},
			1.0f,
			1.0f,
			0.045f,
			0.0075
		};
	}

	void PointLight::Submit() const noexcept(!IS_DEBUG)
	{
		_mMesh.SetPos(_mConstantBufferData.pos);
		_mMesh.Submit();
	}

	void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
	{
		auto dataCopy = _mConstantBufferData;
		const auto pos = DirectX::XMLoadFloat3(&_mConstantBufferData.pos);
		DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
		_mCbuf.Update(gfx, dataCopy);
		_mCbuf.Bind(gfx);
	}
}