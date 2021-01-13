#pragma once
#include "GraphicAPI/Graphics.h"	
#include "RendererAPI/GFXContext.h"
#include "RendererAPI/RenderGraph/RenderQueuePass.h"
#include "RendererAPI/RenderGraph/RenderGraph.h"
#include <vector>
#include <memory>
namespace FraplesDev
{
	class TechniqueProbe;
	class Step
	{
	public:
		Step(std::string targetPassName);
		Step(Step&&) = default;
		Step(const Step& src)noexcept;
		Step& operator=(const Step&) = delete;
		Step& operator=(Step&&) = delete;
		void AddContext(std::shared_ptr<GfxContext>context);
		void Submit(const class Renderer& renderer)const;
		void Bind(Graphics& gfx)const noexcept(!IS_DEBUG);
		void Accept(TechniqueProbe& probe);
		void InitializeParentReferences(const class Renderer& parent)noexcept;
		void Link(RenderGraph& rg);
	private:
		std::vector<std::shared_ptr<GfxContext>>_mPcontexts;
		RenderQueuePass* _mTargetPass = nullptr;
		std::string _mTargetPassName;
	};
}