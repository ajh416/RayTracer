#pragma once

#include "RayTracer.h"
#include "Object.h"
#include "BoundingBox.h"

class Box : public Object
{
public:
	Box(const Bounds3<float>& box, const int&& material_index) : Object(Vec3f(0.0), material_index), m_Box(box) {}

	// Hit detection method generated by ChatGPT!
	virtual bool Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const override;

public:
	Bounds3<float> m_Box;
};