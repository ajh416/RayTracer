#pragma once

#include "RayTracer.h"

class Ray
{
public:
	constexpr Ray() {}

	constexpr Ray(const glm::vec3& origin, const glm::vec3& direction) : Origin(origin), Direction(direction) {}

	constexpr glm::vec3 At(const float t) const { return Origin + (t * Direction); }

public:
	glm::vec3 Origin;
	glm::vec3 Direction;
};
