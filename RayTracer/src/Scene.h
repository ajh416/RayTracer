#pragma once

#include "RayTracer.h"
#include "Shapes/Shape.h"
#include "Vector.h"

#include <vector>

struct Scene
{
	~Scene() {
		for (auto* shape : Shapes) { delete shape; }
	}

	std::vector<Shape*> Shapes;
	std::vector<Material> Materials;
};