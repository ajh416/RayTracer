#include "Triangle.h"

Triangle::Triangle(Vec3f v[3]) {
	for (int i = 0; i < 3; i++) {
		Vertices[i] = v[i];
	}

	Vec3f A = v[1] - v[0];
	Vec3f B = v[2] - v[0];
	Vec3f C = Cross(A, B);
	Normal = Normalize(C);
}

Triangle::Triangle(const Vec3f& point1, const Vec3f& point2, const Vec3f& point3) {
	Vertices[0] = point1;
	Vertices[1] = point2;
	Vertices[2] = point3;

	Vec3f A = point2 - point1;
	Vec3f B = point3 - point1;
	Vec3f C = Cross(A, B);
	Normal = Normalize(C);
}
