#include "Plane.h"

bool Plane::Hit(const Ray<Float>& r, Float tMin, Float tMax, Float& hitDistance) const
{
	// (x, y, z) = (r.Origin.x + r.Direction.x * t, r.Origin.y + r.Direction.y * t, r.Origin.z + r.Direction.z * t)
	// Need three points within/on plane for hit calculation, perhaps use bounding box?
}
