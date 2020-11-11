#pragma once
#include <vector>
#include "GraphicAPI/Graphics.h"
#include "Job.h"
namespace FraplesDev
{
	class Pass
	{
	public:
		void Accept(Job job)noexcept
		{
			_mJobs.push_back(job);
		}
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG)
		{
			for (auto& j : _mJobs)
			{
				j.Execute(gfx);
			}
		}
		void Reset()noexcept
		{
			_mJobs.clear();
		}
	private:
		std::vector<Job>_mJobs;
	};
}