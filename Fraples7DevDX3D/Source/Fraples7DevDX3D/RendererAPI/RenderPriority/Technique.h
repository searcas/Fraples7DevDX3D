#pragma once
#include "Step.h"
#include <vector>
namespace FraplesDev
{
	class Technique
	{
	public:
		Technique(size_t channels);
		Technique(std::string name, size_t channels, bool isActive = true) noexcept;
		void Submit(const class Renderer& renderer, size_t channels)const noexcept;
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
		size_t _mChannel = 0;
	};
}