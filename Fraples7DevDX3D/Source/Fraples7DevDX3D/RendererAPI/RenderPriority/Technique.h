#pragma once
#include "Step.h"
#include <vector>
namespace FraplesDev
{
	class Technique
	{
	public:
		void Submit(class FrameCommander& frame, const class Renderer& renderer)const noexcept;
		void AddStep(Step step)noexcept
		{
			_mSteps.push_back(std::move(step));
		}
		inline void Activate()noexcept
		{
			_mActivate = true;
		}
		inline void Deactivate()noexcept
		{
			_mActivate = false;
		}
		void InitializeParentReferences(const class Renderer& parent)noexcept;
	private:
		bool _mActivate = true;
		std::vector<Step>_mSteps;
	};
}