#include "Triangle.h"

Triangle::Triangle(glm::vec3 v[3], int material_index) : Object(glm::vec3(1.0f), material_index) {
	for (int i = 0; i < 3; i++) {
		Vertices[i] = v[i];
	}

	glm::vec3 A = v[1] - v[0];
	glm::vec3 B = v[2] - v[0];
	glm::vec3 C = glm::cross(A, B);
	Normal = glm::normalize(C);

	Origin = (v[0] + v[1] + v[2]) / 3.0f;
}

Triangle::Triangle(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3, int material_index) : Object(glm::vec3(1.0f), material_index) {
	Vertices[0] = point1;
	Vertices[1] = point2;
	Vertices[2] = point3;

	glm::vec3 A = point2 - point1;
	glm::vec3 B = point3 - point1;
	glm::vec3 C = glm::cross(A, B);
	Normal = glm::normalize(C);

	Origin = (point1 + point2 + point3) / 3.0f;
}

bool Triangle::Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const {
	float NDotRayDirection = glm::dot(Normal, r.Direction);
	if (fabs(NDotRayDirection) < 1e-6) {
		return false;
	}

	float d = -glm::dot(Normal, Vertices[0]);
	hitDistance = -(glm::dot(Normal, r.Origin) + d) / NDotRayDirection;

	if (hitDistance < 0) return false;

	glm::vec3 P = r.At(hitDistance);

	glm::vec3 C;

	glm::vec3 edge0 = Vertices[1] - Vertices[0];
	glm::vec3 vp0 = P - Vertices[0];
	C = glm::cross(edge0, vp0);
	if (glm::dot(Normal, C) < 0) return false;

	glm::vec3 edge1 = Vertices[2] - Vertices[1];
	glm::vec3 vp1 = P - Vertices[1];
	C = glm::cross(edge1, vp1);
	if (glm::dot(Normal, C) < 0) return false;

	glm::vec3 edge2 = Vertices[0] - Vertices[2];
	glm::vec3 vp2 = P - Vertices[2];
	C = glm::cross(edge2, vp2);
	if (glm::dot(Normal, C) < 0) return false;

	return true;
}

void Triangle::MoveTo(const glm::vec3& newOrigin) {
	for (int i = 0; i < 3; i++) {
		Vertices[i] += newOrigin - Origin;
	}
	glm::vec3 A = Vertices[1] - Vertices[0];
	glm::vec3 B = Vertices[2] - Vertices[0];
	glm::vec3 C = glm::cross(A, B);
	Normal = glm::normalize(C);

	Origin = (Vertices[0] + Vertices[1] + Vertices[2]) / 3.0f;
}
