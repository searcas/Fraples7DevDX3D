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
		void SetSinkTarget(const std::string& skinName, const std::string& target);
		void AppendPass(std::unique_ptr<class Pass>pass);
		void Execute(Graphics& gfx)noexcept(!IS_DEBUG);
		void Reset()noexcept;
		void Finalize();
		RenderQueuePass& GetRenderQueue(const std::string& passName);
	private:
		void LinkPassInputs(class Pass& pass);
		void LinkGlobalSinks();
	private:
		std::vector<std::unique_ptr<Pass >> _mPasses;
		std::vector<std::unique_ptr<class PassOutput >> _mGlobalSources;
		std::vector<std::unique_ptr<class PassInput>>_mGlobalSinks;
		std::shared_ptr<class RenderTarget>_mBackBufferTarget;
		std::shared_ptr<class DepthStencil>_mMasterDepth;
		bool finalized = false;
	};
}