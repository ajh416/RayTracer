#pragma once

#include "RayTracer.h"
#include "Objects/Object.h"

struct Scene
{
	~Scene() {
		for (auto* object : Objects) { delete object; }
	}

	std::vector<Object*> Objects;
	std::vector<Material> Materials;
};
