#pragma once
#include "RendererAPI/RenderPriority/Job.h"
#include "PassInput.h"
#include "PassOutput.h"
#include "RenderQueuePass.h"
#include <vector>
namespace FraplesDev
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(std::string name)
			:RenderQueuePass(std::move(name))
		{
			RegisterInput(BufferInput<RenderTarget>::Make("renderTarget",_mRenderTarget));
			RegisterInput(BufferInput<DepthStencil>::Make("renderTarget",_mDepthStencil));
			RegisterOutput(BufferOutput<RenderTarget>::Make("renderTarget", _mRenderTarget));
		}

	private:
		std::vector<Job>_mJobs;
	};
}