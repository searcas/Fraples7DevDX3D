#pragma once
#include "Step.h"
#include <vector>
namespace FraplesDev
{
	class Technique
	{
	public:
		Technique() = default;
		Technique(std::string name, bool isActive = true)noexcept;
		void Submit(const class Renderer& renderer)const noexcept;
		void Link(RenderGraph& rg);
		void AddStep(Step step)noexcept;
		void SetActiveState(bool active)noexcept;
		bool IsActivated()noexcept;
		void InitializeParentReferences(const class Renderer& parent)noexcept;
		void Accept(TechniqueProbe& probe);
		const std::string& GetName()const noexcept;
	private:
		bool _mActivate = true;
		std::vector<Step>_mSteps;
		std::string _mName;
	};
}