#pragma once
#include <vector>
#include <memory>

namespace FraplesDev
{
	class Camera;
	class Graphics;
	class RenderGraph;

	class MultiCamera
	{
	public:
		void SpawnWindow(Graphics& gfx);
		void Bind(Graphics& gfx);
		void AddCamera(std::shared_ptr<Camera>pCam);
		inline Camera& GetControlledCamera() { return *_mCameras[_mControlled]; };
		~MultiCamera() = default;
		void LinnkTechniques(RenderGraph& rg);
		void Submit(size_t channels)const;
		Camera& GetActiveCamera();
		Camera* operator->();
			
	private:
		std::vector<std::shared_ptr<Camera>>_mCameras;
		unsigned int _mActive = 0;
		unsigned int _mControlled = 0;
	};
}