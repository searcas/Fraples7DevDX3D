#pragma once
#include "RenderQueuePass.h"

namespace FraplesDev
{
	class Camera;
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		void BindShadowCamera(const Camera& cam)noexcept;
		ShadowMappingPass(Graphics& gfx, std::string name);
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override;
		void DumpShadowMap(Graphics&, const std::string& path);
	private:
		const Camera* _mShadowCamera = nullptr;
	};
}
