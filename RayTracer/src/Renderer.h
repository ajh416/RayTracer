#pragma once

#include "RayTracer.h"
#include "Vector.h"

class Renderer
{
	Renderer() = delete;
public:
	static void Render();

private:
	static uint32_t PerPixel(Vector3<Float>& coord);
};