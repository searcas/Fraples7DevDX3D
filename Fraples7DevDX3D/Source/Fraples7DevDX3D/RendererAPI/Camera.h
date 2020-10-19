#pragma once
#include "../GraphicAPI/Graphics.h"


namespace FraplesDev
{
	class Camera
	{
	public:
		void Reset();
		void SpawnControllWindow()noexcept;
		DirectX::XMMATRIX GetMatrix() const noexcept;
	private:

			float r = 20.0f;
			float theta = 0.0f;
			float phi = 0.0f;
			float pitch = 0.0f;
			float yaw = 0.0f;
			float roll = 0.0f;


	};


}