#pragma once

#include "RayTracer.h"
#include "Shape.h"
#include "BoundingBox.h"

class Box : public Shape
{
public:
	Box(const Bounds3<Float>& box, const int&& material_index) : Shape(Vec3f(0.0), material_index), m_Box(box) {}
	virtual bool Hit(const Ray<Float>& r, Float tMin, Float tMax, Float& hitDistance) const override;

public:
	Bounds3<Float> m_Box;
};