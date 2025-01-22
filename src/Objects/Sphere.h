#pragma once

#include "RayTracer.h"
#include "Object.h"

class Sphere : public Object
{
public:
	Sphere(glm::vec3 position = glm::vec3(), float radius = 0.5, int material_index = 0) : Object(position, material_index), Position(position), Radius(radius) {}

	virtual bool Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const override
	{
		// Using equation sqrLength(r.Origin + r.Direction * distance) = radius^2
		glm::vec3 oc = r.Origin - Position; // origin of ray - origin of sphere
		auto a = glm::dot(r.Direction, r.Direction); // square the direction
		auto b = 2.0f * glm::dot(oc, r.Direction);
		auto c = glm::dot(oc, oc) - Radius * Radius;
		auto discriminant = b * b - 4.0 * a * c; // quadratic formula
		if (discriminant >= 0.0f) // if we hit the sphere
		{
			float sqrtd = (float)std::sqrt(discriminant);
			float t0 = (-b - sqrtd) / (2.0f * a); // CLOSEST T (SMALLEST)
			//float t1 = (-b + sqrtd) / (2.0 * a); // second "hit' is the ray leaving the object, in our case a sphere

			//glm::vec3 h0 = r.At(t0); // CLOSEST HITPOINT (SMALLEST)
			//glm::vec3 h1 = r.At(t1);
			//if (h0 > h1) std::swap(h0, h1);

			if (t0 < 0)
				return false;
			
			hitDistance = t0;

			return true;
		}

		// we only reach here if we didn't hit the sphere
		//Vector3 unit_direction = Normalize(r.GetDirection()); // the unit vector (magnitude == 1) of the rays direction
		//auto t = 0.5 * (unit_direction.y + 1.0); // make t 0 -> 1
		//return Utils::VectorToUInt32(Utils::Lerp(glm::vec3(1.0), glm::vec3(0.5, 0.7, 1.0), t)); // lerp the values to white -> light blue based on y coord
		return false;
	}

public:
	glm::vec3 Position;
	float Radius;
};