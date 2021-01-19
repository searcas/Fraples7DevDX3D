#include "RenderGraph.h"
#include "RendererAPI/RenderPriority/Pass.h"
#include "RendererAPI/Stencil/DepthStencil.h"
#include "RendererAPI/RenderTarget.h"
#include "RendererAPI/GFXContextBase.h"
#include "Exception/RenderGraphCompileException.h"
#include "Passes/RenderQueuePass.h"
#include "Sync.h"
#include "Source.h"
#include "Core//Surface.h"
#include <sstream>

namespace FraplesDev
{
	RenderGraph::RenderGraph(Graphics& gfx)
		:_mBackBufferTarget(gfx.GetTarget()), _mMasterDepth(std::make_shared<OutputOnlyDepthStencil>(gfx))
	{
		AddGlobalSource(DirectBufferSource<RenderTarget>::Make("backbuffer", _mBackBufferTarget));
		AddGlobalSource(DirectBufferSource<DepthStencil>::Make("masterDepth", _mMasterDepth));
		AddGlobalSink(DirectBufferSync<RenderTarget>::Make("backbuffer", _mBackBufferTarget));
	}
	RenderGraph::~RenderGraph()
	{
	}
	void RenderGraph::SetSinkTarget(const std::string& sinkName, const std::string& target)
	{
		const auto finder = [&sinkName](const std::unique_ptr<Sync>& p) {return p->GetRegisteredName() == sinkName; };
		const auto i = std::find_if(_mGlobalSinks.begin(), _mGlobalSinks.end(), finder);
		if (i == _mGlobalSinks.end())
		{
			throw RGC_EXCEPTION("Global sink does not exists: " + sinkName);
		}
		auto targetSplit = Utility::SplitString(target, ".");
		if (targetSplit.size() !=2u)
		{
			throw RGC_EXCEPTION("Input target has incorrect format");
		}
		(*i)->SetTarget(targetSplit[0], targetSplit[1]);
	}
	void RenderGraph::AppendPass(std::unique_ptr<class Pass> pass)
	{
		assert(!finalized);
		// validate name uniqueness
		for (auto& p : _mPasses)
		{
			if (pass->GetName() == p->GetName())
			{
				throw RGC_EXCEPTION("Pass name already exists: " + pass->GetName());
			}
		}
		// link output from passes (and global outputs) to pass inputs
		LinkSyncs(*pass);

		// add to container of passes 
		_mPasses.push_back(std::move(pass));
	}
	Pass& RenderGraph::FindPassByName(const std::string& name)
	{
		const auto i = std::find_if(_mPasses.begin(), _mPasses.end(), [&name](auto& p)
			{
				return p->GetName() == name;
			});
		if (i == _mPasses.end())
		{
			throw std::runtime_error{ "Filed to find pass name" };
		}
		return **i;
	}
	void RenderGraph::AddGlobalSource(std::unique_ptr<Source>out)
	{
		_mGlobalSources.push_back(std::move(out));
	}
	void RenderGraph::AddGlobalSink(std::unique_ptr<Sync>in)
	{
		_mGlobalSinks.push_back(std::move(in));
	}
	void RenderGraph::Execute(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		assert(finalized);
		for (auto& p : _mPasses)
		{
			p->Execute(gfx);
		}
	}
	void RenderGraph::Reset() noexcept
	{
		assert(finalized);
		for (auto& p : _mPasses)
		{
			p->Reset();
		}
	}
	void RenderGraph::StoreDepth(Graphics& gfx, const std::string& path)
	{
		_mMasterDepth->ToSurface(gfx).Save(path);
	}
	void RenderGraph::Finalize()
	{
		assert(!finalized);
		for (auto& p : _mPasses)
		{
			p->Finalize();
		}
		LinkGlobalSinks();
		finalized = true;
	}
	RenderQueuePass& RenderGraph::GetRenderQueue(const std::string& passName)
	{
		try
		{
			for (const auto& p : _mPasses)
			{
				if (p->GetName() == passName)
				{
					return dynamic_cast<RenderQueuePass&>(*p);
				}
			}
		}
		catch (std::bad_cast&)
		{
			throw RGC_EXCEPTION("In RenderGraph::GetRenderQueue, pass was not RenderQueuePass:" + passName);
		}
		throw RGC_EXCEPTION("In RenderGraph::GetRenderQueue, pass not found: " + passName);
	}
	void RenderGraph::LinkSyncs(Pass& pass)
	{
		for (auto& in : pass.GetSyncs())
		{
			const auto& inputSourcePassName = in->GetPassName();

			//check whether target source is global

			if (inputSourcePassName == "$")
			{
				bool bound = false;
				for (auto& source : _mGlobalSources)
				{
					if (source->GetName()== in->GetSourceName())
					{
						in->Bind(*source);
						bound = true;
						break;
					}
				}
				if (!bound)
				{
					std::ostringstream oss;
					oss << "Output named [" << in->GetSourceName() << "] not found in globals";
					throw RGC_EXCEPTION(oss.str());
				}
			}
			else //find source from within existing passes
			{
				bool bound = false;
				for (auto& existingPass : _mPasses)
				{
					if (existingPass->GetName() == inputSourcePassName)
					{
						auto& source = existingPass->GetSource(in->GetSourceName());
						in->Bind(source);
						bound = true;
						break;
					}
				}
				if (!bound)
				{
					std::ostringstream oss;
					oss << "Pass named [" << inputSourcePassName << "] not found";
					throw RGC_EXCEPTION(oss.str());
				}
			}
		}
	}
	void RenderGraph::LinkGlobalSinks()
	{
		for (auto& sink : _mGlobalSinks)
		{
			const auto& inputSourcePassName = sink->GetPassName();

			for (auto& existingPass : _mPasses)
			{
				if (existingPass->GetName() == inputSourcePassName)
				{
					auto& source = existingPass->GetSource(sink->GetSourceName());
					sink->Bind(source);
					break;
				}
			}
		}
	}

}