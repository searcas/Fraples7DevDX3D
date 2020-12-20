#pragma once
#include "Pass.h"
namespace FraplesDev
{
	Pass::Pass(std::string name) noexcept
	{
	}
	void Pass::Reset() noexcept(!IS_DEBUG)
	{
	}
	const std::vector<std::unique_ptr<PassInput>>& Pass::GetInputs() const
	{
		// TODO: insert return statement here
	}
	PassOutput& Pass::GetOutput(const std::string& registeredName) const
	{
		// TODO: insert return statement here
	}
	PassInput& Pass::GetInput(const std::string& registeredName) const
	{
		// TODO: insert return statement here
	}
	void Pass::SetInputSource(const std::string& registeredName, const std::string& target)
	{
	}
	void Pass::RegisterInput(std::unique_ptr<PassInput> input)
	{
	}
	void Pass::RegisterOutput(std::unique_ptr<PassOutput> ouput)
	{
	}
	void Pass::BindBufferResources(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
	}
	const std::string& Pass::GetName() noexcept
	{
		// TODO: insert return statement here
	}
	void Pass::Finalize()
	{
	}
	void Pass::Accept(Job job) noexcept
	{
	}
	void Pass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG)
	{
	}
}
	