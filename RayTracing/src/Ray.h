#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
	Ray() = default;
	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: Origin(origin), Direction(direction)
	{}

	glm::vec3 HitPoint(float t) const
	{
		return Origin + t * Direction;
	}

public:
	glm::vec3 Origin;
	glm::vec3 Direction;
};
