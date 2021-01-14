#pragma once
#include <string>
#include <vector>
#include <memory>


namespace FraplesDev
{
	class RenderQueuePass;
	class RenderGraph
	{
	public:
		RenderGraph(class Graphics& gfx);
		~RenderGraph();
		void AppendPass(std::unique_ptr<class Pass>pass);
		void AddGlobalSource(std::unique_ptr<class Source>);
		void AddGlobalSink(std::unique_ptr<class Sync>);
		void Execute(Graphics& gfx)noexcept(!IS_DEBUG);
		void SetSinkTarget(const std::string& skinName, const std::string& target);
		void Reset()noexcept;
		void Finalize();
		RenderQueuePass& GetRenderQueue(const std::string& passName);
	private:
		void LinkSyncs(class Pass& pass);
		void LinkGlobalSinks();
	private:
		std::vector<std::unique_ptr<Pass >> _mPasses;
		std::vector<std::unique_ptr<class Source >> _mGlobalSources;
		std::vector<std::unique_ptr<class Sync>>_mGlobalSinks;
		std::shared_ptr<class RenderTarget>_mBackBufferTarget;
		std::shared_ptr<class DepthStencil>_mMasterDepth;
		bool finalized = false;
	};
}