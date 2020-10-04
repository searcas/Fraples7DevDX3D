#pragma once
#include <utility>


constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template<typename T>
constexpr auto Sq(const T&& x)
{
	return std::move(x * x);
}

template <typename T>
constexpr T wrap_angle(T& theta)
{
	const T modded = fmod(theta, (T)2.0 * (T)PI_D);
	return (modded > (T)PI_D) ? (modded - (T)2.0 * (T)PI_D) : modded;
}

template<typename T>
constexpr T interpolate(const T& src, const T& dist, const float& alpha)
{
	return src + (dist - src) * alpha;
}

template <typename T>
constexpr T to_rad(T degree)
{
	return degree * PI / (T)180.0;
}
