#pragma once

#include "RayTracer.h"

#include "Vector.h"
#include "Object.h"

class Triangle : public Object {
public:
	Triangle(Vec3f verts[3], int material_index = 0);
	Triangle(const Vec3f& point1, const Vec3f& point2, const Vec3f& point3, int material_index = 0);

	Vec3f Vertices[3];
	Vec3f Normal;

	bool Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const override;
};