#pragma once

#include "RayTracer.h"

#include "Shapes/Shape.h"

class Mesh : public Shape {
	Mesh(const std::string& filename);

	virtual bool Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const override;
};