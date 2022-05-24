#pragma once

#include "Ray.h"
#include "Material.h"

#include <glm/glm.hpp>
#include <memory>

struct HitRecord
{
	glm::vec3 Point;
	glm::vec3 Normal;
	float T;
	bool FrontFace;
	std::shared_ptr<Material> Material;

	void SetFaceNormal(const Ray& ray, const glm::vec3& outwardNormal)
	{
		FrontFace = glm::dot(ray.Direction, outwardNormal) < 0.0f;
		Normal = FrontFace ? outwardNormal : -outwardNormal;
	}
};
