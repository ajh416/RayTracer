#pragma once

#include "RayTracer.h"
#include "Ray.h"

struct HitPayload
{
	glm::vec3 WorldPosition;
	glm::vec3 WorldNormal;
	float HitDistance;

	int ObjectIndex;
};

struct Material
{
	glm::vec3 Albedo = glm::vec3(1.0f);
	float Roughness = 1.0f;
	float Metallic = 0.0f;

	glm::vec3 EmissionColor = glm::vec3(0.0f);
	float EmissionStrength = 0.0f;
};

enum class ObjectType
{
	Sphere,
	Triangle,
	Plane,
	BoundingBox,
	Mesh
};

class Object
{
public:
	Object(const glm::vec3& origin, int material_index) : Origin(origin), MaterialIndex(material_index) {}
	virtual ~Object() = default;

	virtual bool Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const = 0;

	virtual ObjectType GetType() const = 0;

	glm::vec3 Origin;
	int MaterialIndex = 0;
};
