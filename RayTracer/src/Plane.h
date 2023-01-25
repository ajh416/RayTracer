#pragma once

#include "RayTracer.h"
#include "Shape.h"

class Plane : public Shape
{
public:
	Plane(const Vec3f& normal, const int material_index) : Shape(normal, material_index) {}

	virtual bool Hit(const Ray<Float>& r, Float tMin, Float tMax, Float& hitDistance) const override;
};