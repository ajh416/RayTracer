#pragma once

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <random>

#include "Timer.h"

#include <glm/glm.hpp>

template<typename T>
class Bounds2;
template<typename T>
class Bounds3;

using Bounds2f = Bounds2<float>;
using Bounds3f = Bounds3<float>;

template<typename T>
class Point2;
template<typename T>
class Point3;

using Point2f = Point2<float>;
using Point3f = Point3<float>;

// Useful Constants
static constexpr float Pi = 3.14159265358979323846f;
static constexpr float InvPi = 0.31830988618379067154f;
static constexpr float Inv2Pi = 0.15915494309189533577f;
static constexpr float Inv4Pi = 0.07957747154594766788f;
static constexpr float PiOver2 = 1.57079632679489661923f;
static constexpr float PiOver4 = 0.78539816339744830961f;
static constexpr float Sqrt2 = 1.41421356237309504880f;

#ifndef NDEBUG

#ifdef _WIN32
#define PROFILE_FUNCTION() Timer t##__LINE__(__func__);
#define PROFILE_SCOPE(name) Timer t##__LINE__(#name);
#else
#define PROFILE_FUNCTION() Timer t##__LINE__(__func__);
#define PROFILE_SCOPE(name) Timer t##__LINE__(#name);
#endif // _WIN32

#else // NDEBUG

#define PROFILE_FUNCTION()
#define PROFILE_SCOPE()

#endif // NDEBUG

// Utility Functions
namespace Utils
{
	//// returns a float within [0, 1)
	//inline float Randomfloat()
	//{
	//	return (float)std::random_device{}() / ((unsigned long long)std::numeric_limits<unsigned>::max() + 1);
	//}

	// using this pseudorandom generator for speeds sake
	inline float Randomfloat()
	{
		// seed the engine using a guaranteed random uint32_t
		// when multithreading use thread_local or else all cores will try to use one instance and will heavily impair speed
		static thread_local std::default_random_engine eng(std::random_device{}());
		static std::uniform_real_distribution<float> dis(0.0, 1.0);
		return dis(eng);
	}

	//// returns a float within [low, high)
	//inline float Random(float low, float high)
	//{
	//	return low + ((float)std::random_device{}() / std::numeric_limits<unsigned>::max() + 1) * (high - low);
	//}

	// using this pseudorandom generator for speeds sake
	inline float Random(float low, float high)
	{
		// seed the engine using a guaranteed random uint32_t
		// when multithreading use thread_local or else all cores will try to use one instance and will impair speed
		static thread_local std::default_random_engine eng(std::random_device{}());
		static std::uniform_real_distribution<float> dis(low, high);
		return dis(eng);
	}

	template <typename T, typename U, typename V>
	inline T Clamp(T val, U low, V high) {
		if (val < low) return low;
		else if (val > high) return high;
		else return val;
	}

	template<typename T>
	constexpr inline T Max(T one, T two)
	{
		if (one > two)
			return one;
		else
			return two;
	}

	template <typename T>
	inline T Mod(T a, T b) {
		T result = a - (a / b) * b;
		return (T)((result < 0) ? result + b : result);
	}

	//template <>
	inline float Mod(float a, float b) {
		return std::fmod(a, b);
	}

	inline float Radians(float deg) {
		return (Pi / 180) * deg;
	}
	inline float Degrees(float rad) {
		return (180 / Pi) * rad;
	}

	inline float Log2(float x) {
		constexpr float invLog2 = 1.442695040888963387004650940071f;
		return std::log(x) * invLog2;
	}

#ifdef _WIN32
	inline int Log2Int(uint32_t v) {
		return 31 - __lzcnt(v);
	}
#else
	inline int Log2Int(uint32_t v) {
		return 31 - __builtin_clzl(v);
	}
#endif

	template <typename T>
	inline bool IsPowerOf2(T v) {
		return v && !(v & (v - 1));
	}

	inline int32_t RoundUpPow2(int32_t v) {
		v--;
		v |= v >> 1;    v |= v >> 2;
		v |= v >> 4;    v |= v >> 8;
		v |= v >> 16;
		return v + 1;
	}

#ifdef _WIN32
	inline int CountTrailingZeros(uint32_t v) {
		return _tzcnt_u32(v);
	}
#else
	inline int CountTrailingZeros(uint32_t v) {
		return __builtin_ctzl(v);
	}
#endif
}

//
// Logging Includes and Defines
//

#include "Tools/Logger.h"

#ifdef _WIN32
#define ASSERT(x, ...) if (!x) { ::Logger::Get().Error(__VA_ARGS__); __debugbreak(); }
#else
#define ASSERT(x, ...) if (!x) { ::Logger::Get().Error(__VA_ARGS__); }
#endif

#define ENABLE_LOG

#ifdef ENABLE_LOG

#define LOG_DEBUG(...) ::Logger::Get().Debug(__VA_ARGS__)

#define LOG_INFO(...) ::Logger::Get().Info(__VA_ARGS__)

#define LOG_WARN(...) ::Logger::Get().Warn(__VA_ARGS__)

#define LOG_ERROR(...) ::Logger::Get().Error(__VA_ARGS__)

#ifdef _WIN32
#define LOG_CRITICAL(...) ::Logger::Get().Critical(__VA_ARGS__); __debugbreak();
#else
#define LOG_CRITICAL(...) ::Logger::Get().Critical(__VA_ARGS__);
#endif

#else

#define LOG_INFO(...)

#define LOG_WARN(...)

#define LOG_ERROR(...)

#define LOG_CRITICAL(...)

#endif
