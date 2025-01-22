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

	void Render(const Scene& scene, Camera& cam);

	constexpr void SetSettings(const RenderSettings&& settings) { m_Settings = settings; }

	void SetImage(Image& image);
	void ResetFrameIndex() { m_FrameIndex = 1; }
	uint32_t GetFrameIndex() const { return m_FrameIndex; }

private:
	glm::vec3 PerPixel(const glm::vec2&& coord); // comparable to RayGen shader in GPU ray tracing

	HitPayload TraceRay(const Ray& ray);

	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);

	constexpr HitPayload Miss(const Ray& ray);

	Image* m_Image = nullptr;
	glm::vec3* m_AccumulationData = nullptr;

	std::vector<uint32_t> m_ImageVerticalIter;
	std::vector<uint32_t> m_ImageHorizontalIter;

	Camera* m_Camera = nullptr;
	const Scene* m_Scene = nullptr;

	RenderSettings m_Settings;

	uint32_t m_FrameIndex = 1;
};
