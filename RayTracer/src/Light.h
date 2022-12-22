#pragma once

#include "RayTracer.h"

class Light
{
public:
	Light(const Vec3f& origin) : Origin(origin) {}

public:
	const Vec3f Origin;
};