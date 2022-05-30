#pragma once

#include "Ray.h"

#include <glm/glm.hpp>

class Camera
{
public:
	Camera() = default;
	Camera(const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up, float fov, float aspectRatio, float aperture, float focusDistance);
	~Camera();

	void Set(const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up, float fov, float aspectRatio, float aperture, float focusDistance);

	Ray GetRay(const glm::vec2& uv, bool distortion) const { return distortion ? GetRayWithDistortion(uv.x, uv.y) : GetRay(uv.x, uv.y); }
	Ray GetRay(float u, float v) const;
	Ray GetRayWithDistortion(float u, float v) const;

private:
	glm::vec3 m_Origin;
	glm::vec3 m_LowerLeftCorner;
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
	glm::vec3 m_Forward, m_Upwards, m_Right;
	float m_LensRadius;
};
