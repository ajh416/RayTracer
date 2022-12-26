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
	Vec3f Albedo = { 1.0 };
	Float Roughness = 1.0;
	Float Metallic = 0.0;
};

// TODO: CREATE INTERFACE FOR DETECTING IF THIS SHAPE WAS HIT
class Shape
{
public:
	Shape(const Vec3f& origin, int material_index) : Origin(origin), MaterialIndex(material_index) {}

	virtual bool Hit(const Ray<Float>& r, Float tMin, Float tMax, Float& hitDistance) const = 0;

	Vec3f Origin;
	int MaterialIndex = 0;
};
