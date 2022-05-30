#pragma once

#include "Camera.h"
#include "HitRecord.h"
#include "Components.h"

#include "Walnut/Image.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <memory>

struct RayTracerSpecification
{
	uint32_t Width = 0, Height = 0;
	uint32_t Samples = 1;
	uint32_t Bounces = 8;
};

class RayTracer
{
public:
	RayTracer(const RayTracerSpecification& specs);
	~RayTracer();

	void Resize(uint32_t width, uint32_t height);
	void SetScene(entt::registry& scene) { m_Scene = &scene; }

	void RenderScene(const Camera& camera);
	void UpdateFinalImageData();

public:
	void SetSamples(uint32_t samples) { m_Samples = samples; }
	void SetBounces(uint32_t bounces) { m_Bounces = bounces; }

	const auto& GetFinalImage() const { return m_FinalImage; }
	uint32_t GetSamples() const { return m_Samples; }
	uint32_t GetBounces() const { return m_Bounces; }
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

private:
	void ResizeInternal();
	glm::vec2 GetUV(float x, float y);
	glm::vec4 RayColor(const Ray& ray, int bounces);
	bool CastRay(const Ray& ray, float tMin, float tMax, HitRecord& out_HitRecord);
	bool HitGeometry(const Component::SphereComponent& sphere, const Ray& ray, float tMin, float tMax, HitRecord& hitRecord);

private:
	entt::registry* m_Scene = nullptr;

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_FrameBuffer = nullptr;

	bool m_NeedsResize = false;
	uint32_t m_Width = 0, m_Height = 0;
	uint32_t m_Samples = 1;
	uint32_t m_Bounces = 8;
};
