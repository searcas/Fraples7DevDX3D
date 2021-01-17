#include "Camera.h"
#include "imgui.h"
#include "Core/Math/Math.h"
#include <algorithm>
namespace FraplesDev
{
	void Camera::SpawnControllWindow(Graphics& gfx) noexcept
	{
		bool rotDirty = false;
		bool posDirty = false;
		const auto dcheck = [](bool d, bool& carry) {carry = carry || d; };
		
		ImGui::Text("Camera Position");
		dcheck(ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f"), posDirty);
		ImGui::Text("Orientation");
		dcheck(ImGui::SliderAngle("Pitch", &pitch, 0.955f * -90.0f, 0.955f * 90.0f), rotDirty);
		dcheck(ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f),rotDirty);
		_mProj.RenderWidgets(gfx);
		ImGui::Checkbox("Camera Projection", &_mEnableCameraProjection);
		ImGui::Checkbox("Frustum Projection", &_mEnableFrustumProjection);
		if (ImGui::Button("Reset"))
		{
			Reset(gfx);
		}
		if (rotDirty)
		{
			const DirectX::XMFLOAT3 angles = { pitch,yaw,0.0f };
			_mCamProj.SetRotation(angles);
			_mProj.SetRotation(angles);
		}
		if (posDirty)
		{
			_mCamProj.SetPosition(pos);
			_mProj.SetPosition(pos);
		}
	}

	Camera::Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw) noexcept
		:_mHomePos(homePos), _mHomePitch(homePitch), _mHomeYaw(homeYaw), _mName(std::move(name)),
		_mProj(gfx,1.0f, 9.0f / 16.0f, 0.5f, 400.0f),_mCamProj(gfx)
	{
		Reset(gfx);
	}

	void Camera::Reset(Graphics& gfx)
	{
		pos = _mHomePos;
		pitch = _mHomePitch;
		yaw = _mHomeYaw;

		_mCamProj.SetPosition(pos);
		_mProj.SetPosition(pos);
		const DirectX::XMFLOAT3  angles = { pitch,yaw,0.0f };
		_mCamProj.SetRotation(angles);
		_mProj.SetRotation(angles);
		_mProj.Reset(gfx);
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
		if (_mEnableCameraProjection)
		{
			_mCamProj.Submit();
		}
		if (_mEnableFrustumProjection)
		{
			_mProj.Submit();
		}
	}
}