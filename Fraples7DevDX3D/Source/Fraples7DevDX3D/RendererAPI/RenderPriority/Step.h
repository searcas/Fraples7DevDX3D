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
		Step(const Step& src)noexcept
			:_mTargetPass(src._mTargetPass)
		{
			_mPcontexts.reserve(src._mPcontexts.size());
			for (auto& pb : src._mPcontexts)
			{
				if (auto* pCloning = dynamic_cast<const CloningContext*>(pb.get()))
				{
					_mPcontexts.push_back(pCloning->Clone());
				}
				else
				{
					_mPcontexts.push_back(pb);
				}
			}
		}
		Step& operator=(const Step&) = delete;
		Step& operator=(Step&&) = delete;
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
		void Accept(TechniqueProbe& probe)
		{
			probe.SetStep(this);

			for (auto& pb : _mPcontexts)
			{
				pb->Accept(probe);
			}
		}
		void InitializeParentReferences(const class Renderer& parent)noexcept;
	private:
		size_t _mTargetPass;
		std::vector<std::shared_ptr<GfxContext>>_mPcontexts;
	};
}