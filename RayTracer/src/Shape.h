#pragma once

#include "RayTracer.h"
#include "Vector.h"
#include "Ray.h"

struct HitRecord
{
	Vector3<Float> WorldPosition;
	Vector3<Float> WorldNormal;
	Float HitDistance;

	uint32_t ObjectIndex;
};

// TODO: CREATE INTERFACE FOR DETECTING IF THIS SHAPE WAS HIT
class Shape
{
public:
	virtual bool Hit(const Ray<Float>& r, HitRecord& record) const = 0;
};
