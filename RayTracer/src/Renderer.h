#pragma once

#include "RayTracer.h"

#include "Image.h"
#include "Camera.h"
#include "Vector.h"
#include "Scene.h"

struct RenderSettings
{
	int NumberOfSamples = 1;
	int NumberOfBounces = 1;
	bool Accumulate = true;
	int AccumulateMax = 1;
};

class Renderer
{
public:
	Renderer() = default;
	~Renderer() { delete[] m_AccumulationData; }

	void Render(const Scene& scene, const Camera& cam);

	constexpr void SetSettings(const RenderSettings settings) { m_Settings = settings; }

	void SetImage(Image& image) { m_Image = &image; delete[] m_AccumulationData; m_AccumulationData = new Vec3f[m_Image->Width * m_Image->Height]; }

private:
	Vec3f PerPixel(Vec2f&& coord); // RayGen shader

	HitPayload TraceRay(const Ray<Float>& ray);

	HitPayload ClosestHit(const Ray<Float>& ray, Float hitDistance, int objectIndex);

	constexpr HitPayload Miss(const Ray<Float>& ray);

	Image* m_Image = nullptr;
	Vec3f* m_AccumulationData = nullptr;

	const Camera* m_Camera = nullptr;
	const Scene* m_Scene = nullptr;

	RenderSettings m_Settings;
};