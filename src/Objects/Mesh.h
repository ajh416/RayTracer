#pragma once

#include "RayTracer.h"

#include "Object.h"
#include <Objects/Triangle.h>
#include <Objects/Box.h>

class Mesh : public Object {
public:
	Mesh(const std::string& filename, const int&& material_index);

	virtual bool Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const override;

	virtual ObjectType GetType() const override { return ObjectType::Mesh; }

	void MoveTo(const glm::vec3& newOrigin);

	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> Triangles;
	std::vector<Triangle> MeshTriangles;
	Box BoundingBox;
private:
	bool LoadFromOBJ(const std::string& filename);
	void CalculateBoundingBox();
};
