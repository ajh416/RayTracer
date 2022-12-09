#pragma once

#include "RayTracer.h"

#include "Image.h"
#include "Camera.h"
#include "Vector.h"

class Renderer
{
	Renderer() = delete;
public:
	static void Render(Image& img, Camera& cam);

private:
	static uint32_t PerPixel(Vector2<Float>&& coord);

	static Camera m_Camera;
};