#pragma once
#include "RendererAPI/RenderPriority/Job.h"
#include "RendererAPI/RenderGraph/Sync.h"
#include "RendererAPI/RenderGraph/Source.h"
#include "RendererAPI/Camera/Camera.h"	
#include "RenderQueuePass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/Stencil/Stencil.h"
#include <vector>
namespace FraplesDev
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, std::string&& name)
			:RenderQueuePass(std::move(name))
		{
			RegisterSync(DirectBufferSync<RenderTarget>::Make("renderTarget",_mRenderTarget));
			RegisterSync(DirectBufferSync<DepthStencil>::Make("depthStencil",_mDepthStencil));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}
		void BindMainCamera(const Camera& cam)noexcept
		{
			_mMainCamera = &cam;
		}
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override
		{
			assert(_mMainCamera);
			_mMainCamera->BindGraphics(gfx);
			RenderQueuePass::Execute(gfx);
		}
	private:
		const Camera* _mMainCamera = nullptr;
	};
}