#pragma once

#include "RayTracer.h"
#include "Vector.h"
#include "Ray.h"

struct HitPayload
{
	Vec3f WorldPosition;
	Vec3f WorldNormal;
	Float HitDistance;

	int ObjectIndex;
};

struct Material
{

};

// TODO: CREATE INTERFACE FOR DETECTING IF THIS SHAPE WAS HIT
class Shape
{
public:
	Shape(const Vec3f& origin, const Vec3f& albedo) : Origin(origin), Albedo(albedo) {}

	virtual bool Hit(const Ray<Float>& r, Float tMin, Float tMax, Float& hitDistance) const = 0;

	Vec3f Origin;
	Vec3f Albedo;
};
