#pragma once

#include "RayTracer.h"

#include "Vector.h"
#include "Point.h"

template<typename T>
class Bounds2
{
public:
	Bounds2()
	{
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::lowest();
		pMin = Point2<T>(maxNum, maxNum);
		pMax = Point2<T>(minNum, minNum);
	}

	Bounds2(const Point2<float>& point) : pMin(point), pMax(point)
	{
	}

	Bounds2(const Point2<T>& p1, const Point2<T>& p2)
		: pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
		pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y))
	{
	}

	constexpr Vec2f Center() const
	{
		Vec2f min = Vec2f((float)pMin.x, (float)pMin.y);
		Vec2f max = Vec2f((float)pMax.x, (float)pMax.y);
		auto direction = max - min;
		return min + direction * 0.5;
	}

public:
	Point2<T> pMin, pMax;
};

template<typename T>
class Bounds3
{
public:
	Bounds3()
	{
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::lowest();
		pMin = Point3<T>(maxNum, maxNum, maxNum);
		pMax = Point3<T>(minNum, minNum, minNum);
	}

	Bounds3(const Point3<float>& point) : pMin(point), pMax(point)
	{
	}

	Bounds3(const Point3<T>& p1, const Point3<T>& p2)
		: pMin(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)),
		pMax(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z))
	{
	}

	Vec3f Center() const
	{
		Vec3f min = Vec3f(pMin.x, pMin.y, pMin.z);
		Vec3f max = Vec3f(pMax.x, pMax.y, pMax.z);
		auto direction = max - min;
		return min + direction * 0.5f;
	}

public:
	Point3<T> pMin, pMax;
};