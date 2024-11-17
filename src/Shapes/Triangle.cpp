#include "Triangle.h"

Triangle::Triangle(Vec3f v[3], int material_index) : Shape(Vec3f(1.0f), material_index) {
	for (int i = 0; i < 3; i++) {
		Vertices[i] = v[i];
	}

	Vec3f A = v[1] - v[0];
	Vec3f B = v[2] - v[0];
	Vec3f C = Cross(A, B);
	Normal = Normalize(C);
}

Triangle::Triangle(const Vec3f& point1, const Vec3f& point2, const Vec3f& point3, int material_index) : Shape(Vec3f(1.0f), material_index) {
	Vertices[0] = point1;
	Vertices[1] = point2;
	Vertices[2] = point3;

	Vec3f A = point2 - point1;
	Vec3f B = point3 - point1;
	Vec3f C = Cross(A, B);
	Normal = Normalize(C);
}

bool Triangle::Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const {
	float NDotRayDirection = Dot(Normal, r.Direction);
	if (fabs(NDotRayDirection) < 1e-6) {
		return false;
	}

	float d = -Dot(Normal, Vertices[0]);
	hitDistance = -(Dot(Normal, r.Origin) + d) / NDotRayDirection;

	if (hitDistance < 0) return false;

	Vec3f P = r.At(hitDistance);

	Vec3f C;

	Vec3f edge0 = Vertices[1] - Vertices[0];
	Vec3f vp0 = P - Vertices[0];
	C = Cross(edge0, vp0);
	if (Dot(Normal, C) < 0) return false;

	Vec3f edge1 = Vertices[2] - Vertices[1];
	Vec3f vp1 = P - Vertices[1];
	C = Cross(edge1, vp1);
	if (Dot(Normal, C) < 0) return false;

	Vec3f edge2 = Vertices[0] - Vertices[2];
	Vec3f vp2 = P - Vertices[2];
	C = Cross(edge2, vp2);
	if (Dot(Normal, C) < 0) return false;

	return true;
}
