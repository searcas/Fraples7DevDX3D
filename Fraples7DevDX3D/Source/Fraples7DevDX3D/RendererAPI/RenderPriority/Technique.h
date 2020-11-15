#pragma once
#include "Step.h"
#include <vector>
namespace FraplesDev
{
	class Technique
	{
	public:
		Technique() = default;
		Technique(std::string name, bool isActive = true)noexcept
			:_mName(name),_mActivate(isActive)
		{

		}
		void Submit(class FrameCommander& frame, const class Renderer& renderer)const noexcept;
		void AddStep(Step step)noexcept
		{
			_mSteps.push_back(std::move(step));
		}
		inline void SetActiveState(bool active)noexcept
		{
			_mActivate = active;
		}
		inline bool IsActivated()noexcept
		{
			return _mActivate;
		}
		void InitializeParentReferences(const class Renderer& parent)noexcept;
		void Accept(TechniqueProbe& probe)
		{
			probe.SetTechnique(this);
			for (auto& s : _mSteps)
			{
				s.Accept(probe);
			}
		}
		const std::string& GetName()const noexcept
		{
			return _mName;
		}
	private:
		bool _mActivate = true;
		std::vector<Step>_mSteps;
		std::string _mName = "Nameless Tech";
	};
}