#pragma once

#include "RayTracer.h"

#include "Object.h"

class Mesh : public Object {
	Mesh(const std::string& filename);

	virtual bool Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const override;
};