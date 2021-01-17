#include "Camera.h"
#include "imgui.h"
#include "Core/Math/Math.h"
#include <algorithm>
namespace FraplesDev
{
	void Camera::SpawnControllWindow(Graphics& gfx) noexcept
	{
			ImGui::Text("Camera Position");
			ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Pitch", &pitch, 0.955f * -90.0f, 0.955f * 90.0f);
			ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
			_mProj.RenderWidgets(gfx);
	}

	Camera::Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw) noexcept
		:_mHomePos(homePos), _mHomePitch(homePitch), _mHomeYaw(homeYaw), _mName(std::move(name)),
		_mProj(gfx,1.0f, 9.0f / 16.0f, 0.5f, 400.0f),_mCamProj(gfx)
	{
		Reset();
		_mCamProj.SetPosition(pos);
		_mCamProj.SetRotation({ pitch,yaw,0.0f });
		_mProj.SetPosition(pos);
		_mProj.SetRotation({ pitch,yaw,0.0f });
	}

	void Camera::Reset()
	{
		pos = _mHomePos;
		pitch = _mHomePitch;
		yaw = _mHomeYaw;

	}
	
	DirectX::XMMATRIX Camera::GetMatrix() const noexcept
	{
		using namespace DirectX;
		const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		//apply the camera rotations to a base vector

		const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));

		//generate camera transform (applied to all objects to arrange them relative
		//to camera position/orientation in world from cam position and direction
		//camera top always faces towards +Y (Cannot do a barrel roll)
		const auto camPosition = DirectX::XMLoadFloat3(&pos);
		const auto camTarget = camPosition + lookVector;

		return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	}
	void Camera::Rotate(float dx, float dy) noexcept
	{
		yaw = wrap_angle(yaw + dx * rotationSpeed);
		pitch = std::clamp(pitch + dy * rotationSpeed, 0.955f * -PI / 2.0f, 0.955f * PI / 2.0f);
		const DirectX::XMFLOAT3 angles = { pitch,yaw,0.0f };
		_mCamProj.SetRotation(angles);
		_mProj.SetRotation(angles);
	}
	void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
	{
		DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&translation), DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) * DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)));
		pos = { pos.x + translation.x,
				pos.y + translation.y,
				pos.z + translation.z
				};
		_mCamProj.SetPosition(pos);
		_mProj.SetPosition(pos);
	}
	void Camera::BindGraphics(Graphics& gfx) const
	{
		gfx.SetCamera(GetMatrix());
		gfx.SetProjection(_mProj.GetMatrix());
	}
	void Camera::LinkTechniques(RenderGraph& rg)
	{
		_mCamProj.LinkTechniques(rg);
		_mProj.LinkTechniques(rg);
	}
	void Camera::Submit() const
	{
		_mCamProj.Submit();
		_mProj.Submit();
	}
}