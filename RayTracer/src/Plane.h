#pragma once

#include "RayTracer.h"
#include "Shape.h"

class Plane : public Shape
{
public:
	Plane(const Vec3f& origin, const Float x, const Float y, const int material_index) : Shape(origin, material_index), m_Width(x), m_Height(y) {}

	virtual bool Hit(const Ray<Float>& r, Float tMin, Float tMax, Float& hitDistance) const override;

private:
	Float m_Width;
	Float m_Height;
};