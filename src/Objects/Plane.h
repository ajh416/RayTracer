#pragma once

#include "RayTracer.h"

#include "Object.h"

class Plane : public Object {
public:
	Plane(const Vec3f& point, const Vec3f& normal, int material_index = 0);

	virtual bool Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const override;

private:
	Vec3f m_PointOnPlane;
	Vec3f m_Normal;
};