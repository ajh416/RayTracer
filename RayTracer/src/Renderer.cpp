#include "Renderer.h"

#include "Ray.h"
#include "Vector.h"

void Renderer::Render(const Scene& scene, const Camera& cam)
{
	m_Camera = &cam;
	m_Scene = &scene;

	for (int y = m_Image->Height - 1; y >= 0; y--)
	{
		for (int x = 0; x < (int64_t)m_Image->Width; x++)
		{
			auto color = PerPixel({ Float(x), Float(y) });

			m_Image->Data[x + y * m_Image->Width] = Utils::VectorToUInt32(color);
		}
	}
}

Vec3f Renderer::PerPixel(Vec2f&& coord)
{
	Vector3<Float> res = 0.0;
	for (int i = 0; i < m_Settings.NumberOfSamples; i++)
	{
		auto u = double(coord.x + Utils::RandomFloat()) / (m_Image->Width - 1); // transform the x coordinate to 0 -> 1 (rather than 0 -> image_width)
		auto v = double(coord.y + Utils::RandomFloat()) / (m_Image->Height - 1); // transform the y coordinate to 0 -> 1 (rather than 0 -> image_height)
		const Ray<Float> r = Ray(m_Camera->GetOrigin(), m_Camera->CalculateRayDirection({ u, v }));

		auto payload = TraceRay(r);
		if (payload.HitDistance == -1.0)
		{
			Vector3 unit_direction = Normalize(r.GetDirection()); // the unit vector (magnitude == 1) of the rays direction
			auto t = 0.5 * (unit_direction.y + 1.0); // make t 0 -> 1
			// This is opposite RaytracingInAWeekend, we instead flip the values to lerp to/from because of reversed y UV system
			return Utils::Lerp(Vec3f(0.5, 0.7, 1.0), Vec3f(1.0), t);
		}
		
		Shape* shape = m_Scene->shapes[payload.ObjectIndex];

		Vec3f light_dir = Vec3f(1, -1, 0);
		Float light_intensity = Utils::Max(Dot(payload.WorldNormal, -light_dir), 0.0); // == cos(angle)
		auto color = light_intensity * shape->Albedo;
		color = Utils::Clamp(color, Vec3f(0.0), Vec3f(1.0));

		res += color;
	}

	return res / m_Settings.NumberOfSamples;
}

HitPayload Renderer::TraceRay(const Ray<Float>& ray)
{
	int objectIndex = -1;
	Float hitDistance = std::numeric_limits<Float>::max();

	for (int i = 0; i < m_Scene->shapes.size(); i++)
	{
		const Shape* shape = m_Scene->shapes[i];
		Float newDistance = 0;
		if (shape->Hit(ray, newDistance))
		{
			if (newDistance < hitDistance)
			{
				hitDistance = newDistance;
				objectIndex = i;
			}
		}
	}

	if (objectIndex < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, objectIndex);
}

HitPayload Renderer::ClosestHit(const Ray<Float>& ray, Float hitDistance, int objectIndex)
{
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Shape& closestShape = (Shape&)m_Scene->shapes[objectIndex];

	Vec3f origin = ray.GetOrigin() - closestShape.Origin;
	payload.WorldPosition = origin + ray.GetDirection() * hitDistance;
	payload.WorldNormal = Normalize(payload.WorldPosition);
	payload.WorldPosition += closestShape.Origin;

	return payload;
}

HitPayload Renderer::Miss(const Ray<Float>& ray)
{
	HitPayload payload;
	payload.HitDistance = -1;
	return payload;
}
