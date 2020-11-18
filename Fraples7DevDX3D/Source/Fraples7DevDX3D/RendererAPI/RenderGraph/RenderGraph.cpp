#include "RenderGraph.h"
#include "RendererAPI/RenderPriority/Pass.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/GFXContextBase.h"
#include "Exception/RenderGraphCompileException.h"
#include "RenderQueuePass.h"
#include "PassInput.h"
#include "PassOutput.h"
#include <sstream>

namespace FraplesDev
{
	RenderGraph::RenderGraph(Graphics& gfx)
		:_mBackBufferTarget(gfx.GetTarget())
	{
		_mGlobalSources.push_back(BufferOutput<RenderTarget>::Make("backbuffer", _mBackBufferTarget));
		_mGlobalSources.push_back(BufferOutput<DepthStencil>::Make("masterDepth", _mMasterDepth));

	}
	RenderGraph::~RenderGraph()
	{
	}
	void RenderGraph::SetSinkTarget(const std::string& skinName, const std::string& target)
	{
	}
	void RenderGraph::AppendPass(std::unique_ptr<class Pass> pass)
	{
	}
	void RenderGraph::Execute(Graphics& gfx) noexcept(!IS_DEBUG)
	{
	}
	void RenderGraph::Reset() noexcept
	{
	}
	void RenderGraph::Finalize()
	{
	}
	RenderQueuePass& RenderGraph::GetRenderQueue(const std::string& passName)
	{
		// TODO: insert return statement here
	}
	void RenderGraph::LinkPassInputs(Pass& pass)
	{
	}
	void RenderGraph::LinkGlobalSinks()
	{
	}
}