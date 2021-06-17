#include "PointLight.h"
#include "imgui.h"
#include "RendererAPI/Camera/Camera.h"
namespace FraplesDev
{
	PointLight::PointLight(Graphics& gfx, DirectX::XMFLOAT3 pos,float radius) :
		_mMesh(gfx, radius), _mCbuf(gfx)
	{
		_mHome = { pos,
		{0.05f,0.05,0.05f},
		{1.0f, 1.0f, 1.0f},
		1.0f,
		1.0f,
		0.045f,
		0.0075f
		};
		Reset();
		_mCamera = std::make_shared<Camera>(gfx, "Light Camera", _mConstantBufferData.pos, 0.0f, 0.0f, true);
	}

	void PointLight::SpawnControlWindow() noexcept
	{
		if (ImGui::Begin("Light"))
		{
			bool dirtyPos = false;
			const auto dirtyCheckBoi = [&dirtyPos](bool dirty) {dirtyPos = dirtyPos || dirty; };
			ImGui::Text("Position of Light");
			dirtyCheckBoi(ImGui::SliderFloat("X", &_mConstantBufferData.pos.x, -60.0f, 60.0f, "%.1f"));
			dirtyCheckBoi(ImGui::SliderFloat("Y", &_mConstantBufferData.pos.y, -60.0f, 60.0f, "%.1f"));
			dirtyCheckBoi(ImGui::SliderFloat("Z", &_mConstantBufferData.pos.z, -60.0f, 60.0f, "%.1f"));

			ImGui::Text("Intensity-Color");
			ImGui::SliderFloat("Intensity", &_mConstantBufferData.diffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
			ImGui::ColorEdit3("Diffuse Color", &_mConstantBufferData.diffuseColor.x);
			ImGui::ColorEdit3("Ambient Color", &_mConstantBufferData.ambient.x);


			ImGui::Text("Falloff");
			ImGui::SliderFloat("Constant", &_mConstantBufferData.attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Linear", &_mConstantBufferData.attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("Quadratic", &_mConstantBufferData.attQuad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_Logarithmic);

			if (dirtyPos)
			{
				_mCamera->SetPos(_mConstantBufferData.pos);
			}
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
	std::shared_ptr<Camera> PointLight::ShareCamera() const noexcept
	{
		return _mCamera;
	}
	void PointLight::Reset() noexcept
	{
		_mConstantBufferData = _mHome;
	}

	void PointLight::SetPosition(DirectX::XMFLOAT3 pos) noexcept
	{
		_mConstantBufferData.pos = {pos};
	}

	void PointLight::Submit(size_t channels) const noexcept(!IS_DEBUG)
	{
		_mMesh.SetPos(_mConstantBufferData.pos);
		_mMesh.Submit(channels);
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