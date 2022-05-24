#include "Camera.h"

#include "Walnut/Random.h"

Camera::Camera(const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up, float fov, float aspectRatio, float aperture, float focusDistance)
{
	Set(lookFrom, lookAt, up, fov, aspectRatio, aperture, focusDistance);
}

Camera::~Camera()
{
}

void Camera::Set(const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up, float fov, float aspectRatio, float aperture, float focusDistance)
{
	const float theta = glm::radians(fov);
	const float h = glm::tan(theta * 0.5f);
	const float viewportHeight = h * 2.0f;
	const float viewportWidth = aspectRatio * viewportHeight;

	//m_W = Math::Normalize(lookFrom - lookAt);
	//m_U = Math::Normalize(Math::Cross(up, m_W));
	//m_V = Math::Cross(m_W, m_U);
	m_Forward = glm::normalize(lookAt - lookFrom);
	m_Right = glm::normalize(glm::cross(up, m_Forward));
	m_Upwards = glm::cross(m_Forward, m_Right);

	m_Origin = lookFrom;
	m_Horizontal = focusDistance * viewportWidth * m_Right;
	m_Vertical = focusDistance * viewportHeight * m_Upwards;

	m_LowerLeftCorner = m_Origin - m_Horizontal * 0.5f - m_Vertical * 0.5f + focusDistance * m_Forward;

	m_LensRadius = aperture * 0.5f;
}

Ray Camera::GetRay(float u, float v) const
{
	//glm::vec3 randomVec3 = glm::normalize(glm::vec3(Random::Float(-1.0f, 1.0f), Random::Float(-1.0f, 1.0f), 0.0f)) * Random::Float();
	//glm::vec3 randomVec3 = Walnut::Random::InUnitSphere() * Walnut::Random::Float();
	//randomVec3.z = 0.0f;
	glm::vec3 randomVec3 = glm::vec3(0);

	const glm::vec3 rd = m_LensRadius * randomVec3;
	const glm::vec3 offset = m_Right * rd.x + m_Upwards * rd.y;

	return Ray(
		m_Origin + offset,
		m_LowerLeftCorner + u * m_Horizontal + v * m_Vertical - m_Origin - offset
	);
}

Ray Camera::GetRayWithJitter(float u, float v) const
{
	glm::vec3 randomVec3 = Walnut::Random::InUnitSphere() * Walnut::Random::Float();
	randomVec3.z = 0.0f;

	const glm::vec3 rd = m_LensRadius * randomVec3;
	const glm::vec3 offset = m_Right * rd.x + m_Upwards * rd.y;

	return Ray(
		m_Origin + offset,
		m_LowerLeftCorner + u * m_Horizontal + v * m_Vertical - m_Origin - offset
	);
}
