#pragma once

#include "Walnut/Application.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Components.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Camera.h"

#include <entt/entt.hpp>

class RayTracingLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUIRender() override;

	void Render();
	void Clear();

private:
	void CreateScene();
	void CreateRandomScene();
	void AddSphere(const glm::vec3& pos, float radius, std::shared_ptr<Material> material);

	bool HitGeometry(const Component::SphereComponent& sphere, const Ray& ray, float tMin, float tMax, HitRecord& hitRecord);
	glm::vec4 RayColor(const Ray& ray, int bounces);
	bool CastRay(const Ray& ray, float tMin, float tMax, HitRecord& out_HitRecord);

	void DenoiseImageData();

private:
	std::shared_ptr<Walnut::Image> m_Image;
	uint32_t* m_ImageData = nullptr;
	std::shared_ptr<Walnut::Image> m_DenoisedImage;
	uint32_t* m_DenoisedImageData = nullptr;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
	float m_LastDenoiseTime = 0.0f;

	entt::registry m_Registry;
	Camera m_Camera;

	int m_Bounces = 8;
	int m_Samples = 1;

	int m_HalfDenoiseWindowSize = 1;

	bool m_UseCameraJitter = false;
	bool m_ApplyDenoise = true;
	bool m_ShowDenoisedImage = false;
};
