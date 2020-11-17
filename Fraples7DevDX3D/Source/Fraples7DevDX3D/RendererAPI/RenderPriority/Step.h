#pragma once
#include <vector>
#include <memory>
#include "RendererAPI/GFXContext.h"
#include "GraphicAPI/Graphics.h"	
#include "TechniqueProbe.h"
namespace FraplesDev
{
	class Step
	{
	public:
		Step(Step&&) = default;
		Step(const Step& src)noexcept;
		Step& operator=(const Step&) = delete;
		Step& operator=(Step&&) = delete;
		Step(size_t targetPass_in);
		void AddContext(std::shared_ptr<GfxContext>context);
		void Submint(class FrameCommander& frame, const class Renderer& renderer)const;
		void Bind(Graphics& gfx)const;
		void Accept(TechniqueProbe& probe);
		void InitializeParentReferences(const class Renderer& parent)noexcept;
	private:
		size_t _mTargetPass;
		std::vector<std::shared_ptr<GfxContext>>_mPcontexts;
	};
}