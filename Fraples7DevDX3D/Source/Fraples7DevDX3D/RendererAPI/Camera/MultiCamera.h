#pragma once
#include <vector>
#include <memory>

namespace FraplesDev
{
	class Camera;
	class Graphics;
	class MultiCamera
	{
	public:
		void SpawnWindow();
		void Bind(Graphics& gfx);
		void AddCamera(std::unique_ptr<Camera>pCam);
		Camera& GetCamera();
		~MultiCamera() = default;
	private:
		std::vector<std::unique_ptr<Camera>>_mCameras;
		unsigned int _mSelected = 0;
	};
}