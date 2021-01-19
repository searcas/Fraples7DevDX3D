#include "LambertianPass.h"
#include "RendererAPI/RenderPriority/Job.h"
#include "RendererAPI/RenderGraph/Sync.h"
#include "RendererAPI/RenderGraph/Source.h"
#include "RendererAPI/Camera/Camera.h"	
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/Stencil/Stencil.h"
#include "RendererAPI/ShadowMapping/ShadowCameraCBuf.h"
#include "RendererAPI/ShadowMapping/ShadowSampler.h"
#include <vector>
namespace FraplesDev
{
	LambertianPass::LambertianPass(Graphics& gfx, std::string&& name)
		:RenderQueuePass(std::move(name)), 
		_mShadowCBuf{ std::make_shared<ShadowCameraCBuf>(gfx) },
		_mShadowSampler{ std::make_shared<ShadowSampler>(gfx) }
	{
		AddBind(_mShadowCBuf);
		AddBind(_mShadowSampler);
		RegisterSync(DirectBufferSync<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSync(DirectBufferSync<DepthStencil>::Make("depthStencil", _mDepthStencil));
		AddBindSink<GfxContext>("shadowMap");
		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
	}
	void LambertianPass::BindMainCamera(const Camera& cam)noexcept
	{
		_mMainCamera = &cam;
	}
	void LambertianPass::BindShadowCamera(const Camera& cam)noexcept
	{
		_mShadowCBuf->SetCamera(&cam);
	}
	void LambertianPass::Execute(Graphics& gfx)const noexcept(!IS_DEBUG) 
	{
		assert(_mMainCamera);
		_mMainCamera->BindGraphics(gfx);
		_mShadowCBuf->Update(gfx);
		RenderQueuePass::Execute(gfx);
	}
}