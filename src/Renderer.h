#pragma once

#include "RayTracer.h"

#include "Image.h"
#include "Camera.h"
#include "Scene.h"

struct RenderSettings
{
	int NumberOfSamples = 1;
	int NumberOfBounces = 1;
	bool Accumulate = true;
};

class Renderer
{
public:
	Renderer() = default;
	~Renderer() { delete[] m_AccumulationData; }

	void Render(const Scene& scene, const Camera& cam);

	constexpr void SetSettings(const RenderSettings&& settings) { m_Settings = settings; }

	void SetImage(Image& image);
	void ResetFrameIndex() { m_FrameIndex = 1; }
	uint32_t GetFrameIndex() { return m_FrameIndex; }

private:
	Vec3f PerPixel(const Vec2f&& coord); // comparable to RayGen shader in GPU ray tracing

	HitPayload TraceRay(const Ray<float>& ray);

	constexpr HitPayload ClosestHit(const Ray<float>& ray, float hitDistance, int objectIndex);

	constexpr HitPayload Miss(const Ray<float>& ray);

	Image* m_Image = nullptr;
	Vec3f* m_AccumulationData = nullptr;

	std::vector<uint32_t> m_ImageVerticalIter;
	std::vector<uint32_t> m_ImageHorizontalIter;

	const Camera* m_Camera = nullptr;
	const Scene* m_Scene = nullptr;

	RenderSettings m_Settings;

	uint32_t m_FrameIndex = 1;
};
