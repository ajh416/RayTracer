#pragma once

#include "RayTracer.h"
#include "Vector.h"

template<typename T>
class Ray
{
public:
	constexpr Ray() {}

	constexpr Ray(const Vector3<T>& origin, const Vector3<T>& direction) : m_Origin(origin), m_Direction(direction) {}

	constexpr inline Vector3<T> GetOrigin() const { return m_Origin; }
	constexpr inline Vector3<T> GetDirection() const { return m_Direction; }

	constexpr Vector3<T> At(const Float t) const { return m_Origin + (t * m_Direction); }
private:
	const Vector3<T> m_Origin;
	const Vector3<T> m_Direction;
};
