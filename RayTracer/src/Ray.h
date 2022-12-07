#pragma once

#include "RayTracer.h"
#include "Vector.h"

template<typename T>
class Ray
{
public:
	Ray() {}

	Ray(const Vector3<T>& origin, const Vector3<T>& direction) : m_Origin(origin), m_Direction(direction) {}

	inline Vector3<T> GetOrigin() const { return m_Origin; }
	inline Vector3<T> GetDirection() const { return m_Direction; }

	Vector3<T> At(Float t) const { return m_Origin + t * m_Direction; }
private:
	Vector3<T> m_Origin;
	Vector3<T> m_Direction;
};
