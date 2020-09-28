#pragma once
#include <chrono>



namespace FraplesDev
{
	class Timer
	{
	public:
		Timer();
		float Get()noexcept;
		float Time()noexcept;
	private:
		std::chrono::steady_clock::time_point _mLast;
	};

	
}