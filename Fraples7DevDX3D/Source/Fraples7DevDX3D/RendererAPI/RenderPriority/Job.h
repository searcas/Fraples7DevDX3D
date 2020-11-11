#pragma once


namespace FraplesDev
{
	class Job
	{
	public:
		Job(const class Step* pStep, const class Renderer* pDrawable);
		void Execute(class Graphics& gfx)const noexcept(!IS_DEBUG);
	private:
		const class Renderer* _mPrenderer;
		const class Step* _mpStep;
	};

}