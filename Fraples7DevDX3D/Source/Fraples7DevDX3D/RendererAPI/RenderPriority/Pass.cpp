#pragma once
#include "Pass.h"
namespace FraplesDev
{
	void Pass::Accept(Job job)noexcept
	{
		_mJobs.push_back(job);
	}
	void Pass::Execute(Graphics& gfx)const noexcept(!IS_DEBUG)
	{
		for (auto& j : _mJobs)
		{
			j.Execute(gfx);
		}
	}
	void Pass::Reset()noexcept
	{
		_mJobs.clear();
	}
}