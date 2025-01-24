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

	constexpr glm::vec2 Center() const
	{
		glm::vec2 min = glm::vec2((float)pMin.x, (float)pMin.y);
		glm::vec2 max = glm::vec2((float)pMax.x, (float)pMax.y);
		auto direction = max - min;
		return min + direction * 0.5f;
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

	glm::vec3 Center() const
	{
		glm::vec3 min = glm::vec3(pMin.x, pMin.y, pMin.z);
		glm::vec3 max = glm::vec3(pMax.x, pMax.y, pMax.z);
		auto direction = max - min;
		return min + direction * 0.5f;
	}

public:
	Point3<T> pMin, pMax;
};
