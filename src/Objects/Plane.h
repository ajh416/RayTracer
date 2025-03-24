#pragma once

#include "RayTracer.h"

#include "Object.h"

class Plane : public Object {
public:
	Plane(const glm::vec3& point, const glm::vec3& normal, int material_index = 0);

	virtual bool Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const override;

	virtual ObjectType GetType() const override { return ObjectType::Plane; }
private:
	glm::vec3 m_PointOnPlane;
	glm::vec3 m_Normal;
};
