#pragma once

#include "RayTracer.h"

#include "Image.h"
#include "Camera.h"
#include "Vector.h"
#include "Scene.h"

class Renderer
{
	Renderer() = delete;
public:
	static void Render(const Scene& scene, Image& img, const Camera& cam);

private:
	static uint32_t TraceRay(const Scene& scene, Vector2<Float>&& coord);

	static uint32_t PerPixel(Vector2<Float>&& coord);

	static Camera m_Camera;
};