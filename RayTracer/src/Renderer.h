#pragma once

#include "RayTracer.h"

#include "Image.h"
#include "Camera.h"
#include "Vector.h"
#include "Scene.h"

struct RenderSettings
{
	int NumberOfSamples = 1;
};

class Renderer
{
public:
	Renderer() = default;

	void Render(const Scene& scene, const Camera& cam);

	void SetSettings(const RenderSettings settings) { m_Settings = settings; }

	void SetImage(Image* image) { m_Image = image; }

private:
	Vec3f PerPixel(Vec2f&& coord); // RayGen shader

	HitPayload TraceRay(const Ray<Float>& ray);

	HitPayload ClosestHit(const Ray<Float>& ray, Float hitDistance, int objectIndex);

	HitPayload Miss(const Ray<Float>& ray);

	Image* m_Image = nullptr;
	const Camera* m_Camera = nullptr;
	const Scene* m_Scene = nullptr;

	RenderSettings m_Settings;
};