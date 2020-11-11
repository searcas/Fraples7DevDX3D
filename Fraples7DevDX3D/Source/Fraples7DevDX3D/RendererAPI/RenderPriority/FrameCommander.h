#pragma once
#include <array>
#include "RendererAPI/GFXContextBase.h"
#include "GraphicAPI/Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "RendererAPI/Stencil.h"
#include "RendererAPI/NullPixelShader.h"
#include "Utility/SpeedLog.h"
namespace FraplesDev
{
	class FrameCommander
	{
	public:
		void Accept(Job job, size_t target)noexcept
		{
			_mPasses[target].Accept(job);
		}
		void Execute(Graphics& gfx)const noexcept(!IS_DEBUG)
		{
			// Normally this would be a loop with each pass
			// defining it setup / etc. and later on it would
			// be a complex graph with parallel execution contingent
			// on input / output requirements

			// main phong lighting pass
			Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
			_mPasses[0].Execute(gfx);
			
			//outline masking pass
			Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
			NullPixelShader::Resolve(gfx)->Bind(gfx);
			_mPasses[1].Execute(gfx);

			// Outline drawing pass
			Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);

			
			_mPasses[2].Execute(gfx);
		}
		void Reset()noexcept
		{
			for (auto& p : _mPasses)
			{
				p.Reset();
			}
		}
	private:
		std::array<Pass, 3>_mPasses;
	};
}