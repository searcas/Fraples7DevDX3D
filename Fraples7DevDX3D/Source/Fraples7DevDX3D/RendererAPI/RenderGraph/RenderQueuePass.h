#pragma once
#include "BindingPass.h"
#include "GraphicAPI/Graphics.h"
#include "RendererAPI/RenderPriority/Job.h"
namespace FraplesDev
{
	class RenderQueuePass : public BindingPass
	{
	public:
		using BindingPass::BindingPass;
		void Accept(Job job)noexcept;
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG) override;
		void Reset()noexcept(!IS_DEBUG) override;

	private:
		std::vector<Job>_mJobs;
	};
}