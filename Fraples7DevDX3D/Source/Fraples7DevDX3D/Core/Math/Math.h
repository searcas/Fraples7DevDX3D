#pragma once
#include <utility>


constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template<typename T>
constexpr auto Sq(const T&& x)noexcept
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
	else
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
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}