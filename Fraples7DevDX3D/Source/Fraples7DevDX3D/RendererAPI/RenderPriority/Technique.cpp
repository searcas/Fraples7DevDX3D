#include "Technique.h"
#include "RendererAPI/Renderer.h"
#include "TechniqueProbe.h"
namespace FraplesDev
{
	void Technique::Submit(const Renderer& renderer, size_t channelsFilter) const noexcept
	{
		if (_mActivate && ((_mChannel  & channelsFilter) != 0))
		{
			for (const auto& step : _mSteps)
			{
				step.Submit(renderer);
			}
		}
	}
	void Technique::Link(RenderGraph& rg)
	{
		for (auto& step : _mSteps)
		{
			step.Link(rg);
		}
	}
	void Technique::InitializeParentReferences(const Renderer& parent) noexcept
	{
		for (auto& s : _mSteps)
		{
			s.InitializeParentReferences(parent);
		}
	}

	Technique::Technique(std::string name, size_t channels, bool isActive )noexcept
		:_mName(name), _mActivate(isActive),_mChannel(channels)
	{

	}
	Technique::Technique(size_t channels)
		: _mChannel(channels)
	{

	}
	void Technique::AddStep(Step step)noexcept
	{
		_mSteps.push_back(std::move(step));
	}
	void Technique::SetActiveState(bool active)noexcept
	{
		_mActivate = active;
	}
	bool Technique::IsActivated()noexcept
	{
		return _mActivate;
	}
	void Technique::Accept(TechniqueProbe& probe)
	{
		probe.SetTechnique(this);
		for (auto& s : _mSteps)
		{
			s.Accept(probe);
		}
	}
	const std::string& Technique::GetName()const noexcept
	{
		return _mName;
	}
}