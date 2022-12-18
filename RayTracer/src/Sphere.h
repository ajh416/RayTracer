#pragma once

#include "RayTracer.h"
#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(Vector3<Float> position = { 0 }, Float radius = 0.5) : m_Position(position), m_Radius(radius) {}

	virtual bool Hit(const Ray<Float>& r, HitRecord& record) const override
	{
		Vector3<Float> oc = r.GetOrigin() - m_Position; // origin of ray - origin of sphere
		auto a = Dot(r.GetDirection(), r.GetDirection()); // square the direction
		auto b = 2.0 * Dot(oc, r.GetDirection());
		auto c = Dot(oc, oc) - m_Radius * m_Radius;
		auto discriminant = b * b - 4.0 * a * c; // quadratic formula
		if (discriminant >= 0.0) // if we hit the sphere
		{
			auto sqrtd = sqrt(discriminant);
			double t0 = (-b - sqrtd) / (2.0 * a); // CLOSEST T (SMALLEST)
			double t1 = (-b + sqrtd) / (2.0 * a); // second "hit' is the ray leaving the object, in our case a sphere

			Vector3<Float> h0 = r.At(t0); // CLOSEST HITPOINT (SMALLEST)
			Vector3<Float> h1 = r.At(t1);
			//if (h0 > h1) std::swap(h0, h1);

			const Vector3<Float> normal = Normalize(h0 - m_Position);
			Vector3<Float> light_dir = Normalize(Vector3<Float>(0, -1, 0));
			Float light_intensity = Utils::Max(Dot(normal, -light_dir), 0.0); // == cos(angle)
			auto color = light_intensity * Vector3<Float>(1.0, 0.5, 0.4);
			color = Utils::Clamp(color, Vector3(0.0), Vector3(1.0));

			record.WorldPosition = this->m_Position;
			record.WorldNormal = normal;
			record.HitDistance = t0;

			return true;
		}

		// we only reach here if we didn't hit the sphere
		//Vector3 unit_direction = Normalize(r.GetDirection()); // the unit vector (magnitude == 1) of the rays direction
		//auto t = 0.5 * (unit_direction.y + 1.0); // make t 0 -> 1
		//return Utils::VectorToUInt32(Utils::Lerp(Vector3<Float>(1.0), Vector3<Float>(0.5, 0.7, 1.0), t)); // lerp the values to white -> light blue based on y coord
		return false;
	}

private:
	Vector3<Float> m_Position;
	Float m_Radius;
};