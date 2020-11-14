#include "Camera.h"
#include "imgui.h"
#include "Core/Math/Math.h"
#include <algorithm>
namespace FraplesDev
{
	void Camera::SpawnControllWindow() noexcept
	{
		if (ImGui::Begin("Camera"))
		{
			ImGui::Text("Camera Position");
			ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
			ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Pitch", &pitch, 0.955f * -90.0f, 0.955f * 90.0f);
			if (ImGui::Button("Reset"))
			{
				Reset();
			}
		}

			ImGui::End();
	}

	Camera::Camera() noexcept
	{
		Reset();
	}

	void Camera::Reset()
	{
		pos = { 0.0f,0.5f, -12.0f };
		pitch = 0.0f;
		yaw = 0.0f;

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

	}
	void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
	{
		DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&translation), DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) * DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)));
		pos = { pos.x + translation.x,
				pos.y + translation.y,
				pos.z + translation.z
				};
	}
}