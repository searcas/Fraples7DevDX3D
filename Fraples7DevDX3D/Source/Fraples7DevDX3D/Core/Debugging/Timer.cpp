#include "Timer.h"




namespace FraplesDev
{
	Timer::Timer()
	{
		_mLast = std::chrono::steady_clock::now();
	}

	float Timer::Start() noexcept
	{
		const auto& old = _mLast;
		_mLast  = std::chrono::steady_clock::now();
		const std::chrono::duration<float> frameTime = _mLast - old;
		return frameTime.count();
	}

	float Timer::Time() noexcept
	{
		return std::chrono::duration<float>(std::chrono::steady_clock::now() - _mLast).count();

	}

}