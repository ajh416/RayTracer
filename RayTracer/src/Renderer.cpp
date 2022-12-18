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

Vector3<Float> Renderer::TraceRay(const Ray<Float>& ray)
{
	HitRecord hit = { 0 };
	for (const Shape* shape : m_Scene->shapes)
	{
		if (shape->Hit(ray, hit))
		{
			return Vector3(0.8, 0.4, 0.4);
		}
	}

	Vector3 unit_direction = Normalize(ray.GetDirection()); // the unit vector (magnitude == 1) of the rays direction
	auto t = 0.5 * (unit_direction.y + 1.0); // make t 0 -> 1

	// This is opposite RaytracingInAWeekend, we instead flip the values to lerp to/from because of reversed y UV system
	return Utils::Lerp(Vector3<Float>(0.5, 0.7, 1.0), Vector3<Float>(1.0), t);
}

Vector3<Float> Renderer::PerPixel(Vector2<Float>&& coord)
{
	Vector3<Float> res = 0.0;
	for (int i = 0; i < m_Settings.NumberOfSamples; i++)
	{
		auto u = double(coord.x + Utils::RandomFloat()) / (m_Image->Width - 1); // transform the x coordinate to 0 -> 1 (rather than 0 -> image_width)
		auto v = double(coord.y + Utils::RandomFloat()) / (m_Image->Height - 1); // transform the y coordinate to 0 -> 1 (rather than 0 -> image_height)
		const Ray<Float> r = Ray(m_Camera->GetOrigin(), m_Camera->GetLowerLeftCorner() + u * m_Camera->GetHorizontal() + v * m_Camera->GetVertical() - m_Camera->GetOrigin());
		res += TraceRay(r);
	}

	return res / m_Settings.NumberOfSamples;
}
