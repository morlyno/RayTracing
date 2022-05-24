#pragma once

#include "Ray.h"

struct HitRecord;

class Material
{
public:
	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, glm::vec4& out_Attenuation, Ray& out_Scattered) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const glm::vec4& albedo);
	~Lambertian();

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, glm::vec4& out_Attenuation, Ray& out_Scattered) const override;

private:
	glm::vec4 m_Albedo;
};


class Metal : public Material
{
public:
	Metal(const glm::vec4& albedo, float fuzz);
	~Metal();

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, glm::vec4& out_Attenuation, Ray& out_Scattered) const override;

private:
	glm::vec4 m_Albedo;
	float m_Fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(float refractionIndex);
	~Dielectric();

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, glm::vec4& out_Attenuation, Ray& out_Scattered) const override;

private:
	static float Refelctance(float cosine, float refractionIndex);

private:
	float m_RefractionIndex;
};

