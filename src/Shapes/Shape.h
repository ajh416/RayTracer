#pragma once

#include "RayTracer.h"
#include "Vector.h"
#include "Ray.h"

struct HitPayload
{
	Vec3f WorldPosition;
	Vec3f WorldNormal;
	float HitDistance;

	int ObjectIndex;
};

struct Material
{
	Vec3f Albedo = { 1.0 };
	float Roughness = 1.0;
	float Metallic = 0.0;

	Vec3f EmissionColor = 0.0;
	float EmissionStrength = 0.0;
};

class Shape
{
public:
	Shape(const Vec3f& origin, int material_index) : Origin(origin), MaterialIndex(material_index) {}

	virtual bool Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const = 0;

	Vec3f Origin;
	int MaterialIndex = 0;
};
