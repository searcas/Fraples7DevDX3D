#pragma once
#include "../GraphicAPI/Graphics.h"


namespace FraplesDev
{
	class Camera
	{
	public:
		Camera(DirectX::XMFLOAT3 homePos,float homePitch, float homeYaw)noexcept;
		void Reset();
		void SpawnControllWindow() noexcept;
		DirectX::XMMATRIX GetMatrix() const noexcept;
		void Rotate(float dx, float dy) noexcept;
		void Translate(DirectX::XMFLOAT3 translation) noexcept;
		inline DirectX::XMFLOAT3 GetPos()const noexcept { return pos; }
	private:
		DirectX::XMFLOAT3 _mHomePos;
		float _mHomePitch;
		float _mHomeYaw;

		DirectX::XMFLOAT3 pos;
		static constexpr float travelSpeed = 12.0f;
		static constexpr float rotationSpeed = 0.004;
		float pitch = 0.0f;
		float yaw = 0.0f;

	};


}