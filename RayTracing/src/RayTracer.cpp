#include "RayTracer.h"
#include "Walnut\Random.h"
#include <iostream>

namespace utils {

	static uint32_t ConvertColorVec4ToUInt32(const glm::vec4& color)
	{
		return
			(uint8_t)(color.x * 255) << 0 |
			(uint8_t)(color.y * 255) << 8 |
			(uint8_t)(color.z * 255) << 16 |
			(uint8_t)(color.w * 255) << 24;
	}

}

RayTracer::RayTracer(const RayTracerSpecification& specs)
	: m_Width(specs.Width), m_Height(specs.Height), m_Samples(specs.Samples), m_Bounces(specs.Bounces)
{
	ResizeInternal();
}

RayTracer::~RayTracer()
{
	if (m_FrameBuffer)
		delete[] m_FrameBuffer;

}

void RayTracer::Resize(uint32_t width, uint32_t height)
{
	m_Width = width;
	m_Height = height;
	m_NeedsResize = true;
}

void RayTracer::RenderScene(const Camera& camera)
{
	if (!m_Scene)
	{
		std::cout << "[RayTracer] Scene not set\n";
		return;
	}

	if (m_NeedsResize)
		ResizeInternal();

	std::cout << "[RayTracer] RenderScene [Bounces: " << m_Bounces << ", Samples: " << m_Samples << "]\n";

	for (uint32_t y = 0; y < m_Height; y++)
	{
		for (uint32_t x = 0; x < m_Width; x++)
		{
			Ray ray = camera.GetRay(GetUV(x, y), false);
			glm::vec4 color = RayColor(ray, m_Bounces);

			if (m_Samples > 1)
			{
				for (uint32_t i = 1; i < m_Samples; i++)
				{
					const float sampleRadius = 1.0f;
					const float u = ((float)x + Walnut::Random::Float(-sampleRadius, sampleRadius)) / (m_Width - 1);
					const float v = 1.0f - (((float)y + Walnut::Random::Float(-sampleRadius, sampleRadius)) / (m_Height - 1));

					Ray ray = camera.GetRay(u, v);
					color += RayColor(ray, m_Bounces);
				}

				color /= (float)m_Samples;
			}

			const uint32_t index = y * m_Width + x;
			m_FrameBuffer[index] = utils::ConvertColorVec4ToUInt32(color) | 0xff000000;
		}
	}

	m_FinalImage->SetData(m_FrameBuffer);

	std::cout << "[RayTracer] Render Finished\n";
}

void RayTracer::UpdateFinalImageData()
{
	m_FinalImage->SetData(m_FrameBuffer);
}

void RayTracer::ResizeInternal()
{
	if (m_FinalImage &&
		(m_Width == 0 || m_Height == 0 ||
		m_Width == m_FinalImage->GetWidth() && m_Height == m_FinalImage->GetHeight()))
		return;

	m_FinalImage = std::make_shared<Walnut::Image>(m_Width, m_Height, Walnut::ImageFormat::RGBA);

	delete[] m_FrameBuffer;
	m_FrameBuffer = new uint32_t[m_Width * m_Height];

	m_NeedsResize = false;
}

glm::vec2 RayTracer::GetUV(float x, float y)
{
	if (m_Samples == 1)
	{
		return {
			(float)x / (m_Width - 1),
			1.0f - ((float)y / (m_Height - 1))
		};
	}

	return {
		((float)x + Walnut::Random::Float(-1.0f, 1.0f)) / (m_Width - 1),
		1.0f - (((float)y + Walnut::Random::Float(-1.0f, 1.0f)) / (m_Height - 1))
	};
}

glm::vec4 RayTracer::RayColor(const Ray& ray, int bounces)
{
	if (bounces <= 0)
		return glm::vec4(0.0f);

	HitRecord hitRecord;
	if (CastRay(ray, 0.001f, FLT_MAX, hitRecord))
	{
		Ray scattered;
		glm::vec4 attenuation;
		if (hitRecord.Material && hitRecord.Material->Scatter(ray, hitRecord, attenuation, scattered))
			return attenuation * RayColor(scattered, bounces - 1);
		return glm::vec4(0.0f);
	}

	glm::vec3 dir = glm::normalize(ray.Direction);
	float t = 0.5f * (dir.y + 1.0f);
	return (1.0f - t) * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) + t * glm::vec4(0.5f, 0.7f, 1.0f, 1.0f);
}

bool RayTracer::CastRay(const Ray& ray, float tMin, float tMax, HitRecord& out_HitRecord)
{
	HitRecord hitRecord;
	bool hit = false;
	float closest = tMax;

	auto view = m_Scene->view<Component::SphereComponent>();
	for (auto entity : view)
	{
		auto& sphereComp = view.get<Component::SphereComponent>(entity);
		if (HitGeometry(sphereComp, ray, tMin, closest, hitRecord))
		{
			if (m_Scene->all_of<Component::MaterialComponent>(entity))
			{
				auto& materialComp = m_Scene->get<Component::MaterialComponent>(entity);
				hitRecord.Material = materialComp.Material;
			}

			hit = true;
			closest = hitRecord.T;
			out_HitRecord = hitRecord;
		}
	}

	return hit;
}

bool RayTracer::HitGeometry(const Component::SphereComponent& sphere, const Ray& ray, float tMin, float tMax, HitRecord& hitRecord)
{
	glm::vec3 oc = ray.Origin - sphere.Center;
	float a = glm::dot(ray.Direction, ray.Direction);
	float bHalf = glm::dot(oc, ray.Direction);
	float c = glm::dot(oc, oc) - sphere.Radius * sphere.Radius;
	float discriminat = bHalf * bHalf - a * c;

	if (discriminat < 0.0f)
		return false;

	float d = glm::sqrt(discriminat);
	float r = (-bHalf - d) / a;
	if (r < tMin || r > tMax)
	{
		r = (-bHalf + d) / a;
		if (r < tMin || r > tMax)
			return false;
	}

	hitRecord.Point = ray.HitPoint(r);
	hitRecord.SetFaceNormal(ray, (hitRecord.Point - sphere.Center) / sphere.Radius);
	hitRecord.T = r;
	hitRecord.Material = nullptr;

	return true;
}
