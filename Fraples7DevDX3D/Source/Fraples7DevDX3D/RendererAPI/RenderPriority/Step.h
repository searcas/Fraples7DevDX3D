#pragma once
#include <vector>
#include <memory>
#include "RendererAPI/GFXContext.h"
#include "GraphicAPI/Graphics.h"	

namespace FraplesDev
{
	class Step
	{
	public:
		Step(size_t targetPass_in)
			:_mTargetPass{ targetPass_in }
		{
			
		}
		void AddContext(std::shared_ptr<GfxContext>context)
		{
			_mPcontexts.push_back(std::move(context));
		}
		void Submint(class FrameCommander& frame, const class Renderer& renderer)const;
		void Bind(Graphics& gfx)const
		{
			for (auto& b : _mPcontexts)
			{
				b->Bind(gfx);
			}
		}
		void InitializeParentReferences(const class Renderer& parent)noexcept;
	private:
		size_t _mTargetPass;
		std::vector<std::shared_ptr<GfxContext>>_mPcontexts;
	};
}