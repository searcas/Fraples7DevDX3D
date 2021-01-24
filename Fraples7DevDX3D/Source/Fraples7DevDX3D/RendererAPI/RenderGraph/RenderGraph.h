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
		void Reset()noexcept;
		void Execute(Graphics& gfx)noexcept(!IS_DEBUG);
		RenderQueuePass& GetRenderQueue(const std::string& passName);
	protected:
		void AppendPass(std::unique_ptr<class Pass>pass);
		Pass& FindPassByName(const std::string& name);
		void AddGlobalSource(std::unique_ptr<class Source>);
		void AddGlobalSink(std::unique_ptr<class Sync>);
		void SetSinkTarget(const std::string& skinName, const std::string& target);
		void Finalize();
		std::shared_ptr<class RenderTarget>_mBackBufferTarget;
		std::shared_ptr<class DepthStencil>_mMasterDepth;
	private:
		void LinkSyncs(class Pass& pass);
		void LinkGlobalSinks();
	private:
		std::vector<std::unique_ptr<Pass >> _mPasses;
		std::vector<std::unique_ptr<class Source >> _mGlobalSources;
		std::vector<std::unique_ptr<class Sync>>_mGlobalSinks;
		
		bool finalized = false;
	};
}