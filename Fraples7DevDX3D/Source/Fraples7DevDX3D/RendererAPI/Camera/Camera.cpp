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
		
		if (!tethered)
		{
			ImGui::Text("Camera Position");
			dcheck(ImGui::SliderFloat("X", &_mPos.x, -80.0f, 80.0f, "%.1f"), posDirty);
			dcheck(ImGui::SliderFloat("Y", &_mPos.y, -80.0f, 80.0f, "%.1f"), posDirty);
			dcheck(ImGui::SliderFloat("Z", &_mPos.z, -80.0f, 80.0f, "%.1f"), posDirty);
		}
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
			_mCamProj.SetPosition(_mPos);
			_mProj.SetPosition(_mPos);
		}
	}

	Camera::Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw, bool tethered) noexcept
		:_mHomePos(homePos), _mHomePitch(homePitch), _mHomeYaw(homeYaw), _mName(std::move(name)),
		_mProj(gfx,1.0f, 9.0f / 16.0f, 0.5f, 400.0f),_mCamProj(gfx),tethered(tethered)
	{
		if (tethered)
		{
			_mPos = homePos;
			_mCamProj.SetPosition(_mPos);
			_mProj.SetPosition(_mPos);
		}
		Reset(gfx);
	}

	void Camera::Reset(Graphics& gfx)
	{
		if (!tethered)
		{
			_mPos = _mHomePos;
			_mCamProj.SetPosition(_mPos);
			_mProj.SetPosition(_mPos);
		}
		pitch = _mHomePitch;
		yaw = _mHomeYaw;

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
		const auto camPosition = DirectX::XMLoadFloat3(&_mPos);
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
		if (!tethered)
		{
			DirectX::XMStoreFloat3(&translation, 
			DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation), 
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) * 
			DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)));
			_mPos = { _mPos.x + translation.x,
					_mPos.y + translation.y,
					_mPos.z + translation.z
			};
			_mCamProj.SetPosition(_mPos);
			_mProj.SetPosition(_mPos);
		}
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
	void Camera::SetPos(const DirectX::XMFLOAT3& pos)
	{
		_mPos = pos;
		_mCamProj.SetPosition(pos);
		_mProj.SetPosition(pos);
	}
}