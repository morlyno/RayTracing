#include "Material.h"

#include "HitRecord.h"

#include "Walnut/Random.h"

#include <glm/geometric.hpp>
#include <glm/gtc/epsilon.hpp>

Lambertian::Lambertian(const glm::vec4& albedo)
	: m_Albedo(albedo)
{
}

Lambertian::~Lambertian()
{
}

bool Lambertian::Scatter(const Ray& ray, const HitRecord& hitRecord, glm::vec4& out_Attenuation, Ray& out_Scattered) const
{
	glm::vec3 scatterDirection = hitRecord.Normal + Walnut::Random::Vec3();

	if (glm::all(glm::epsilonEqual(scatterDirection, glm::vec3(0.0f), 0.0001f)))
		scatterDirection = hitRecord.Normal;

	out_Scattered = Ray(hitRecord.Point, scatterDirection);
	out_Attenuation = m_Albedo;

	return true;
}

Metal::Metal(const glm::vec4& albedo, float fuzz)
	: m_Albedo(albedo), m_Fuzz(std::min(fuzz, 1.0f))
{
}

Metal::~Metal()
{
}

bool Metal::Scatter(const Ray& ray, const HitRecord& hitRecord, glm::vec4& out_Attenuation, Ray& out_Scattered) const
{
	glm::vec3 reflected = glm::reflect(glm::normalize(ray.Direction), hitRecord.Normal);
	out_Scattered = Ray(hitRecord.Point, reflected + m_Fuzz * (Walnut::Random::InUnitSphere() * Walnut::Random::Float()));
	out_Attenuation = m_Albedo;
	return glm::dot(out_Scattered.Direction, hitRecord.Normal) > 0;
}

Dielectric::Dielectric(float refractionIndex)
	: m_RefractionIndex(refractionIndex)
{
}

Dielectric::~Dielectric()
{
}

bool Dielectric::Scatter(const Ray& ray, const HitRecord& hitRecord, glm::vec4& out_Attenuation, Ray& out_Scattered) const
{
	out_Attenuation = glm::vec4(1);
	float refrationRatio = hitRecord.FrontFace ? (1.0f / m_RefractionIndex) : m_RefractionIndex;

	glm::vec3 dir = glm::normalize(ray.Direction);

	const float cosTheta = glm::min(glm::dot(-dir, hitRecord.Normal), 1.0f);
	const float sinTheta = glm::sqrt(1.0f - cosTheta * cosTheta);

	bool cannotRefract = (refrationRatio * sinTheta) > 1.0f;

	glm::vec3 direction;
	if (cannotRefract || Refelctance(cosTheta, refrationRatio) > Walnut::Random::Float())
		direction = glm::reflect(dir, hitRecord.Normal);
	else
		direction = glm::refract(dir, hitRecord.Normal, refrationRatio);

	out_Scattered = Ray(hitRecord.Point, direction);
	return true;
}

float Dielectric::Refelctance(float cosine, float refractionIndex)
{
	float r0 = (1.0f - refractionIndex) / (1.0f + refractionIndex);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * powf(1.0f - cosine, 5);
}
