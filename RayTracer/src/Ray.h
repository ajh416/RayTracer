#pragma once

#include "RayTracer.h"
#include "Vector.h"

template<typename T>
class Ray
{
public:
	constexpr Ray() {}

	constexpr Ray(const Vector3<T>& origin, const Vector3<T>& direction) : Origin(origin), Direction(direction) {}

	constexpr Vector3<T> At(const float t) const { return Origin + (t * Direction); }

public:
	Vector3<T> Origin;
	Vector3<T> Direction;
};
