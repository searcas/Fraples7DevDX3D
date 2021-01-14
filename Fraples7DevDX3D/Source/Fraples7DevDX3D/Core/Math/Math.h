#pragma once
#include <utility>


constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template<typename T = double>
constexpr auto Sq( T&& x)
{
	return std::move(x * x);
}

template <typename T = double>
inline T wrap_angle(T  theta )noexcept
{
	constexpr T twoPi = (T)2 * (T)PI_D;
	const T mod = fmod(theta, twoPi);
	if (mod > (T)PI_D)
	{
		return mod - twoPi;
	}
	else if (mod < -(T)PI_D)
	{
		mod + twoPi;
	}
	return mod;
}

template<typename T>
constexpr T interpolate(const T& src, const T& dist, const float& alpha)noexcept
{
	return src + (dist - src) * alpha;
}

template <typename T>
constexpr T to_rad(T degree)noexcept
{
	return degree * PI / (T)180.0;
}

// https://en.wikipedia.org/wiki/Gaussian_function
// Scary func
template<typename T>
constexpr T gauss(T x, T sigma) 
{
	const auto ss = Sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-Sq(x) / ((T)2.0 * ss));
}