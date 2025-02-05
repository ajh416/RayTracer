#pragma once

#include "RayTracer.h"

#include "Object.h"

class Triangle : public Object {
public:
	Triangle(glm::vec3 verts[3], int material_index = 0);
	Triangle(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3, int material_index = 0);

	glm::vec3 Vertices[3];
	glm::vec3 Normal;

	bool Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const override;

	virtual ObjectType GetType() override { return ObjectType::Triangle; }
};
