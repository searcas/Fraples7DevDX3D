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
		virtual void Execute(Graphics& gfx)const noexcept(!IS_DEBUG);
		virtual void Reset()noexcept;
	private:
		std::vector<Job>_mJobs;
	};
}