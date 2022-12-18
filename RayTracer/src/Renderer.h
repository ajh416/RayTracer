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
	Renderer() {};
public:
	static void Render(const Scene& scene, Image& img, const Camera& cam);

	static void SetSettings(const RenderSettings settings) { m_Settings = settings; }

private:
	static Vector3<Float> TraceRay(const Scene& scene, Vector2<Float>&& coord);

	static uint32_t PerPixel(Vector2<Float>&& coord);

	static Camera m_Camera;

	static RenderSettings m_Settings;
};