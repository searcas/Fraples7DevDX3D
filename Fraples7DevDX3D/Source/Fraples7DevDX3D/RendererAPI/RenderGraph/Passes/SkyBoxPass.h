#pragma once
#include "RendererAPI/RenderGraph/Passes/BindingPass.h"


namespace FraplesDev
{
	class Camera;
	class SkyBoxPass : public BindingPass
	{
	public:
		SkyBoxPass(Graphics& gfx,std::string name);
		void BindMainCamera(const Camera& cam)noexcept;
		void Execute(Graphics& gfx)const noexcept override;
	private:
		UINT _mCount;
		const Camera* _mMainCam = nullptr;
	};
}