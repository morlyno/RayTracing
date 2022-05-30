#include "RayTracingLayer.h"

namespace utils {

	static uint32_t ConvertColorVec4ToUInt32(const glm::vec4& color)
	{
		return
			(uint8_t)(color.x * 255) <<  0 |
			(uint8_t)(color.y * 255) <<  8 |
			(uint8_t)(color.z * 255) << 16 |
			(uint8_t)(color.w * 255) << 24;
	}

}

void RayTracingLayer::OnAttach()
{
	RayTracerSpecification specs;
	specs.Width = m_ViewportWidth;
	specs.Height = m_ViewportHeight;
	specs.Samples = m_Samples;
	specs.Bounces = m_Bounces;
	m_RayTracer = std::make_shared<RayTracer>(specs);
}

void RayTracingLayer::OnDetach()
{
	delete[] m_DenoisedImageData;
}

void RayTracingLayer::OnUIRender()
{
	ImGui::Begin("Settings");
	ImGui::Text("Last render: %.3fms", m_LastRenderTime);
	ImGui::Text("Last render: %.3fs", m_LastRenderTime / 1000.0f);
	ImGui::Text("Last denoise: %.3fs", m_LastDenoiseTime);

	if (ImGui::Button("Render"))               Render();
	if (ImGui::Button("Denoise"))              DenoiseImageData();
	if (ImGui::Button("Clear"))                Clear();
	if (ImGui::Button("Create Scene"))         CreateScene();
	if (ImGui::Button("Create Random Scene"))  CreateRandomScene();

	if (ImGui::DragInt("Bounces", &m_Bounces, 1.0f, 1, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp)) m_RayTracer->SetBounces(m_Bounces);
	if (ImGui::DragInt("Sambles", &m_Samples, 1.0f, 1, 16, "%d", ImGuiSliderFlags_AlwaysClamp)) m_RayTracer->SetSamples(m_Samples);
	//ImGui::DragInt("Denoise Window", &m_HalfDenoiseWindowSize, 1.0f, 1, INT_MAX);
	//ImGui::Checkbox("Show Denoised Image", &m_ShowDenoisedImage);

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");

	m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
	m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

	auto& image = m_RayTracer->GetFinalImage();
	ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() });

	ImGui::End();
	ImGui::PopStyleVar();
}

void RayTracingLayer::Render()
{
	Walnut::Timer timer;

	if (m_ViewportWidth != m_RayTracer->GetWidth() || m_ViewportHeight != m_RayTracer->GetHeight())
	{
		const float aspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;
		//const glm::vec3 origin = { 0.0f, 0.0f, 10.0f };
		const glm::vec3 origin = { 13.0f, 2.0f, 3.0f };
		const glm::vec3 focusPoint = { 0.0f, 0.0f, 0.0f };
		const glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		m_Camera.Set(origin, focusPoint, up, 20.0f, aspectRatio, 0.1f, 10.0f);

		m_RayTracer->Resize(m_ViewportWidth, m_ViewportHeight);

		m_DenoisedImage = std::make_shared<Walnut::Image>(m_ViewportWidth, m_ViewportHeight, Walnut::ImageFormat::RGBA);
		delete[] m_DenoisedImageData;
		m_DenoisedImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
	}

	m_RayTracer->SetScene(m_Registry);
	m_RayTracer->RenderScene(m_Camera);

	if (m_ApplyDenoise)
		DenoiseImageData();
	else
	{
		memset(m_DenoisedImageData, 0, (uint64_t)m_ViewportWidth * m_ViewportHeight * sizeof(uint32_t));
		m_DenoisedImage->SetData(m_DenoisedImageData);
	}

	m_LastRenderTime = timer.ElapsedMillis();
}

void RayTracingLayer::Clear()
{
	return;

	//if (!m_Image || m_ViewportWidth != m_Image->GetWidth() || m_ViewportHeight != m_Image->GetHeight())
	//{
	//	const float aspectRatio = (float)m_ViewportWidth / (float)m_ViewportHeight;
	//	//const glm::vec3 origin = { 0.0f, 0.0f, 10.0f };
	//	const glm::vec3 origin = { 13.0f, 2.0f, 3.0f };
	//	const glm::vec3 focusPoint = { 0.0f, 0.0f, 0.0f };
	//	const glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	//	m_Camera.Set(origin, focusPoint, up, 20.0f, aspectRatio, 0.1f, 10.0f);
	//
	//	m_Image = std::make_shared<Walnut::Image>(m_ViewportWidth, m_ViewportHeight, Walnut::ImageFormat::RGBA);
	//	m_DenoisedImage = std::make_shared<Walnut::Image>(m_ViewportWidth, m_ViewportHeight, Walnut::ImageFormat::RGBA);
	//	delete[] m_ImageData;
	//	m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
	//	m_DenoisedImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
	//}
	//
	//memset(m_ImageData, 0, (uint64_t)m_ViewportWidth * m_ViewportHeight * sizeof(uint32_t));
	//memset(m_DenoisedImageData, 0, (uint64_t)m_ViewportWidth * m_ViewportHeight * sizeof(uint32_t));
	//
	//m_Image->SetData(m_ImageData);
	//m_DenoisedImage->SetData(m_DenoisedImageData);
}

void RayTracingLayer::CreateScene()
{
	m_Registry.clear();

	//entt::entity sphere0 = m_Registry.create();
	//Component::SphereComponent& sphereComp = m_Registry.emplace<Component::SphereComponent>(sphere0);
	//sphereComp.Center = glm::vec3(0.0f);
	//sphereComp.Radius = 1.0f;
	//
	//auto& materialComp = m_Registry.emplace<Component::MaterialComponent>(sphere0);
	//materialComp.Material = std::make_shared<Lambertian>(glm::vec4(0.9f, 0.3f, 0.1f, 1.0f));



	auto material0 = std::make_shared<Dielectric>(1.5f);
	entt::entity shpere0 = m_Registry.create();
	m_Registry.emplace<Component::SphereComponent>(shpere0, glm::vec3(0.0f, 0.5f, 0.0f), 1.0f);
	m_Registry.emplace<Component::MaterialComponent>(shpere0, material0);
	
	auto material1 = std::make_shared<Lambertian>(glm::vec4(0.4f, 0.2f, 0.1f, 1.0f));
	entt::entity shpere1 = m_Registry.create();
	m_Registry.emplace<Component::SphereComponent>(shpere1, glm::vec3(-4.0f, 0.0f, 0.0f), 1.0f);
	m_Registry.emplace<Component::MaterialComponent>(shpere1, material1);
	
	auto material2 = std::make_shared<Metal>(glm::vec4(0.7f, 0.6f, 0.5f, 1.0f), 0.0f);
	entt::entity shpere2 = m_Registry.create();
	m_Registry.emplace<Component::SphereComponent>(shpere2, glm::vec3(4.0f, 0.0f, 0.0f), 1.0f);
	m_Registry.emplace<Component::MaterialComponent>(shpere2, material2);
}

void RayTracingLayer::CreateRandomScene()
{
	m_Registry.clear();

	auto ground_material = std::make_shared<Lambertian>(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	AddSphere(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, ground_material);

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Walnut::Random::Float();
			glm::vec3 center(a + 0.9f * Walnut::Random::Float(), 0.2f, b + 0.9f * Walnut::Random::Float());

			if (glm::length(center - glm::vec3{ 4.0f, 0.2f, 0.0f }) > 0.9f) {
				std::shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8f) {
					// diffuse
					auto albedo = glm::vec4(Walnut::Random::Vec3() * Walnut::Random::Vec3(), 1.0f);
					sphere_material = std::make_shared<Lambertian>(albedo);
					AddSphere(center, 0.2f, sphere_material);
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = glm::vec4(Walnut::Random::Vec3(0.5f, 1.0f), 1.0f);
					auto fuzz = Walnut::Random::Float(0.0f, 0.5f);
					sphere_material = std::make_shared<Metal>(albedo, fuzz);
					AddSphere(center, 0.2f, sphere_material);
				}
				else {
					// glass
					sphere_material = std::make_shared<Dielectric>(1.5f);
					AddSphere(center, 0.2f, sphere_material);
				}
			}
		}
	}

	auto material1 = std::make_shared<Dielectric>(1.5f);
	AddSphere(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, material1);

	auto material2 = std::make_shared<Lambertian>(glm::vec4(0.4f, 0.2f, 0.1f, 1.0f));
	AddSphere(glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f, material2);

	auto material3 = std::make_shared<Metal>(glm::vec4(0.7f, 0.6f, 0.5f, 1.0f), 0.0f);
	AddSphere(glm::vec3(4.0f, 1.0f, 0.0f), 1.0f, material3);
}

void RayTracingLayer::AddSphere(const glm::vec3& pos, float radius, std::shared_ptr<Material> material)
{
	entt::entity entity = m_Registry.create();
	m_Registry.emplace<Component::SphereComponent>(entity, pos, radius);
	m_Registry.emplace<Component::MaterialComponent>(entity, material);
}

glm::vec4 RayTracingLayer::RayColor(const Ray& ray, int bounces)
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

bool RayTracingLayer::CastRay(const Ray& ray, float tMin, float tMax, HitRecord& out_HitRecord)
{
	HitRecord hitRecord;
	bool hit = false;
	float closest = tMax;

	auto view = m_Registry.view<Component::SphereComponent>();
	for (auto entity : view)
	{
		auto& sphereComp = view.get<Component::SphereComponent>(entity);
		if (HitGeometry(sphereComp, ray, tMin, closest, hitRecord))
		{
			if (m_Registry.all_of<Component::MaterialComponent>(entity))
			{
				auto& materialComp = m_Registry.get<Component::MaterialComponent>(entity);
				hitRecord.Material = materialComp.Material;
			}

			hit = true;
			closest = hitRecord.T;
			out_HitRecord = hitRecord;
		}
	}

	return hit;
}

bool RayTracingLayer::HitGeometry(const Component::SphereComponent& sphere, const Ray& ray, float tMin, float tMax, HitRecord& hitRecord)
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

void RayTracingLayer::DenoiseImageData()
{
	Walnut::Timer timer;

	//const auto indexData = [this](int x, int y) -> uint32_t
	//{
	//	if (x < 0 || y < 0 || x >= (int)m_ViewportWidth || y >= (int)m_ViewportHeight)
	//		return 0;
	//
	//	return m_ImageData[y * m_ViewportWidth + x];
	//};
	//
	//const int halfWindowSize = m_HalfDenoiseWindowSize;
	//const int windowSize = 2 * halfWindowSize + 1;
	//
	//std::vector<uint32_t> window(windowSize * windowSize);
	//
	//for (int y = 0; y < m_ViewportHeight; y++)
	//{
	//	for (int x = 0; x < m_ViewportWidth; x++)
	//	{
	//		uint32_t index = 0;
	//		for (int wy = -halfWindowSize; wy <= halfWindowSize; wy++)
	//		{
	//			for (int wx = -halfWindowSize; wx <= halfWindowSize; wx++)
	//			{
	//				window[index++] = indexData(x + wx, y + wy);
	//			}
	//		}
	//
	//		std::sort(window.begin(), window.end());
	//
	//		m_DenoisedImageData[y * m_ViewportWidth + x] = window[(windowSize * windowSize) / 2];
	//	}
	//}
	//
	//m_DenoisedImage->SetData(m_DenoisedImageData);

	m_LastDenoiseTime = timer.ElapsedMillis();
}

void RayTracingLayer::PrintScene()
{

}
