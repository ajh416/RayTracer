#pragma once

#include "RayTracer.h"
#include "Object.h"

class Sphere : public Object
{
public:
	Sphere(Vec3f position = { 0 }, float radius = 0.5, int material_index = 0) : Shape(position, material_index), Position(position), Radius(radius) {}

	virtual bool Hit(const Ray<float>& r, float tMin, float tMax, float& hitDistance) const override
	{
		// Using equation sqrLength(r.Origin + r.Direction * distance) = radius^2
		Vec3f oc = r.Origin - Position; // origin of ray - origin of sphere
		auto a = Dot(r.Direction, r.Direction); // square the direction
		auto b = 2.0f * Dot(oc, r.Direction);
		auto c = Dot(oc, oc) - Radius * Radius;
		auto discriminant = b * b - 4.0 * a * c; // quadratic formula
		if (discriminant >= 0.0f) // if we hit the sphere
		{
			float sqrtd = (float)std::sqrt(discriminant);
			float t0 = (-b - sqrtd) / (2.0f * a); // CLOSEST T (SMALLEST)
			//float t1 = (-b + sqrtd) / (2.0 * a); // second "hit' is the ray leaving the object, in our case a sphere

			//Vec3f h0 = r.At(t0); // CLOSEST HITPOINT (SMALLEST)
			//Vec3f h1 = r.At(t1);
			//if (h0 > h1) std::swap(h0, h1);

			if (t0 < 0)
				return false;
			
			hitDistance = t0;

			return true;
		}

		// we only reach here if we didn't hit the sphere
		//Vector3 unit_direction = Normalize(r.GetDirection()); // the unit vector (magnitude == 1) of the rays direction
		//auto t = 0.5 * (unit_direction.y + 1.0); // make t 0 -> 1
		//return Utils::VectorToUInt32(Utils::Lerp(Vector3<float>(1.0), Vector3<float>(0.5, 0.7, 1.0), t)); // lerp the values to white -> light blue based on y coord
		return false;
	}

public:
	Vec3f Position;
	float Radius;
};