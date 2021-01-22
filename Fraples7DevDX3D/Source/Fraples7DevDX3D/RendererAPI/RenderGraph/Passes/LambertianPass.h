#pragma once
#include "RenderQueuePass.h"
#include <memory>
namespace FraplesDev
{
	class Camera;
	class Graphics;
	class ShadowCameraCBuf;
	class ShadowSampler;
	class GfxContext;

	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, std::string&& name);
		
		void BindMainCamera(const Camera& cam)noexcept;
		void BindShadowCamera(const Camera& cam)noexcept;
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override;
	private:
		std::shared_ptr<ShadowCameraCBuf>_mShadowCBuf;
		const Camera* _mMainCamera = nullptr;
	};
}