#pragma once

#include "RayTracer.h"

#include "Object.h"
#include <Objects/Triangle.h>

class Mesh : public Object {
public:
	Mesh(const std::string& filename, const int&& material_index);

	virtual bool Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const override;

	virtual ObjectType GetType() const override { return ObjectType::Mesh; }

	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> Triangles;
	std::vector<Triangle> MeshTriangles;
private:
	bool LoadFromOBJ(const std::string& filename);
};
