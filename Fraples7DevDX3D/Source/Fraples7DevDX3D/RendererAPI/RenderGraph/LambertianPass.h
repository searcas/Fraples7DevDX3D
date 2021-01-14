#pragma once
#include "RendererAPI/RenderPriority/Job.h"
#include "Sync.h"
#include "Source.h"
#include "RenderQueuePass.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/Stencil.h"
#include <vector>
namespace FraplesDev
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, std::string&& name)
			:RenderQueuePass(std::move(name))
		{
			RegisterSync(DirectBufferSnyc<RenderTarget>::Make("renderTarget",_mRenderTarget));
			RegisterSync(DirectBufferSnyc<DepthStencil>::Make("depthStencil",_mDepthStencil));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", _mRenderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", _mDepthStencil));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}
	};
}