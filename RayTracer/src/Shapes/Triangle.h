#pragma once

#include "RayTracer.h"

#include "Vector.h"

class Triangle {
public:
	Triangle(Vec3f verts[3]);
	Triangle(const Vec3f& point1, const Vec3f& point2, const Vec3f& point3);

	Vec3f Vertices[3];
	Vec3f Normal;
};