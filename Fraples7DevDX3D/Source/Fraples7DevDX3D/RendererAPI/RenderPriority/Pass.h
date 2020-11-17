#pragma once
#include <vector>
#include "GraphicAPI/Graphics.h"
#include "Job.h"
namespace FraplesDev
{
	class Pass
	{
	public:
		void Accept(Job job)noexcept;
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG);
		void Reset()noexcept;
	private:
		std::vector<Job>_mJobs;
	};
}