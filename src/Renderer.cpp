#include "Renderer.h"

#include "Ray.h"

#include <string.h>

#if RT_LINUX
#include <array>
#include <future>
#endif

#define RT_WINDOWS 1
#if RT_WINDOWS
#include <execution>
#include <algorithm>
#endif

void Renderer::Render(const Scene &scene, Camera &cam) {
	if (m_Camera == nullptr || m_Scene == nullptr) {
		m_Camera = &cam;
		m_Scene = &scene;
	}

	if (m_FrameIndex == 1) {
		memset(m_AccumulationData, 0, m_Image->Width * m_Image->Height * sizeof(glm::vec3));
	}

#define MT 1
#if MT
	// A good bit faster than using my 8 thread version below
	// This (sometimes) doesn't work on linux apparently
	// something something libtbb
	std::for_each(std::execution::par_unseq, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), [this](uint32_t y) {
			std::for_each(std::execution::par_unseq, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
					[this, y](uint32_t x) {
					auto color = PerPixel({ (float)x, (float)y });
					m_AccumulationData[x + y * this->m_Image->Width] += color;
					auto accumulated_color = m_AccumulationData[x + y * this->m_Image->Width];
					accumulated_color /= (float)m_FrameIndex;
					accumulated_color = glm::clamp(accumulated_color, glm::vec3(0.0f), glm::vec3(1.0f));
					m_Image->Data[x + y * this->m_Image->Width] = Utils::Vec3ToUInt32(accumulated_color);
					});
			});

#else // MT

	for (int y = 0; y < (int)m_Image->Height; y++) {
		for (int x = 0; x < (int)m_Image->Width; x++) {
			auto color = PerPixel({ (float)x, (float)y });
			m_AccumulationData[x + y * this->m_Image->Width] += color;
			auto accumulated_color = m_AccumulationData[x + y * this->m_Image->Width];
			accumulated_color /= (float)m_FrameIndex;
			accumulated_color = glm::clamp(accumulated_color, glm::vec3(0.0f), glm::vec3(1.0f));
			m_Image->Data[x + y * this->m_Image->Width] = Utils::VectorToUInt32(accumulated_color);
		}
	}

#endif // MT
	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

void Renderer::SetImage(Image &image) {
	m_Image = &image;
	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec3[m_Image->Width * m_Image->Height];

	m_ImageVerticalIter.resize(m_Image->Height);
	m_ImageHorizontalIter.resize(m_Image->Width);

	for (uint32_t x = 0; x < m_Image->Height; x++)
		m_ImageVerticalIter[x] = x;

	for (uint32_t y = 0; y < m_Image->Width; y++)
		m_ImageHorizontalIter[y] = y;
}

glm::vec3 Renderer::PerPixel(const glm::vec2 &&coord) {
	glm::vec3 res = glm::vec3(0.0f);

	for (int i = 0; i < m_Settings.NumberOfSamples; i++) {
		glm::vec3 bounce_res = glm::vec3(0.0f);
		glm::vec3 ray_color = glm::vec3(1.0f);

		Ray r = Ray(m_Camera->GetPosition(), m_Camera->GetRayDirection({coord.x, coord.y}));

		for (int i = 0; i < m_Settings.NumberOfBounces + 1; i++) {
			auto payload = TraceRay(r);
			if (payload.HitDistance < 0) // did not hit object
				break;

			const Object *Object = m_Scene->Objects[payload.ObjectIndex];
			const Material &material = m_Scene->Materials[Object->MaterialIndex];

			glm::vec3 emitted_light = material.EmissionColor * material.EmissionStrength;
			bounce_res += emitted_light * ray_color;
			ray_color *= material.Albedo;
			if (glm::length(ray_color) < 0.01f && i > 2) { 
				break; // Avoid unimportant rays wasting computation
			}

			r.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
			r.Direction =
				glm::reflect(r.Direction, glm::normalize(payload.WorldNormal + material.Roughness * Utils::RandomVector(-1.0f, 1.0f)));
		}

		res += bounce_res;
	}

	return glm::clamp(res / (float)(m_Settings.NumberOfSamples), glm::vec3(0.0f), glm::vec3(1.0f));
}

HitPayload Renderer::TraceRay(const Ray &ray) {
	int objectIndex = -1;
	float hitDistance = std::numeric_limits<float>::max();

	for (int i = 0; i < m_Scene->Objects.size(); i++) {
		const Object *object = m_Scene->Objects[i];
		float newDistance = 0;

		if (object->Hit(ray, 0, hitDistance, newDistance)) {
			if (newDistance > 0.0 && newDistance < hitDistance) {
				hitDistance = newDistance;
				objectIndex = i;
			}
		}
	}

	if (objectIndex < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, objectIndex);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float hitDistance, int objectIndex) {
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Object *closestObject = m_Scene->Objects[objectIndex];

	glm::vec3 origin = ray.Origin - closestObject->Origin;
	payload.WorldPosition = origin + hitDistance * ray.Direction;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);
	payload.WorldPosition += closestObject->Origin;

	return payload;
}

constexpr HitPayload Renderer::Miss(const Ray &ray) {
	constexpr HitPayload payload = {.HitDistance = -1};
	return payload;
}
