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
	Vector3<Float> PerPixel(Vector2<Float>&& coord);

	Vector3<Float> TraceRay(const Ray<Float>& ray);



	Image* m_Image = nullptr;
	const Camera* m_Camera = nullptr;
	const Scene* m_Scene = nullptr;

	RenderSettings m_Settings;
};