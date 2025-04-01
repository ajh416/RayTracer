#pragma once

#include "RayTracer.h"

#include "Image.h"
#include "Camera.h"
#include "Scene.h"

#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"

struct RenderSettings
{
	int NumberOfSamples = 1;
	int NumberOfBounces = 1;
	bool Accumulate = true;
};

class Renderer
{
public:
	Renderer() = default;
	~Renderer() { delete[] m_AccumulationData; delete m_RenderTexture; }

	void Render(const Scene& scene, Camera& cam);
	void RenderGPU(const Scene& scene, const Camera& cam);

	constexpr void SetSettings(const RenderSettings&& settings) { m_Settings = settings; }

	void SetImage(Image& image);
	void ResetFrameIndex() { m_FrameIndex = 1; }
	void SetRenderGPU(bool gpu) {
		m_RenderGPU = gpu;
	}
	uint32_t GetFrameIndex() const { return m_FrameIndex; }
	uint32_t GetRenderID() const {
		if (!m_RenderGPU) return m_RenderTexture->GetRendererID();
		else return m_FramebufferTexture;
	}

private:
	glm::vec3 PerPixel(const glm::vec2&& coord); // comparable to RayGen shader in GPU ray tracing

	HitPayload TraceRay(const Ray& ray);

	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);

	constexpr HitPayload Miss(const Ray& ray);

	void SetupGPUBuffers(const Scene& scene);

	Image* m_Image = nullptr;
	glm::vec3* m_AccumulationData = nullptr;

	std::vector<uint32_t> m_ImageVerticalIter;
	std::vector<uint32_t> m_ImageHorizontalIter;

	Camera* m_Camera = nullptr;
	const Scene* m_Scene = nullptr;

	Texture* m_RenderTexture = new Texture(0, 0);

	RenderSettings m_Settings;

	uint32_t m_FrameIndex = 1;

	bool m_RenderGPU = false;
	bool m_GPUSetup = false;

	// GPU data
	struct TriangleGPU {
		glm::vec4 v0;   // 16 bytes
		glm::vec4 v1;   // 16 bytes
		glm::vec4 v2;   // 16 bytes
		glm::vec4 normal; // 16 bytes
		int materialIndex; // 4 bytes
		int padding[3];  // 12 bytes to ensure 16-byte alignment
	};
	
	struct MeshGPU {
		glm::vec4 minBounds, maxBounds;  // Use vec4 instead of vec3
		int startTriangleIndex;
		int triangleCount;
		int padding[2];  // Add padding for alignment
	};
	
	struct MaterialGPU {
		glm::vec3 Albedo;
		float Roughness;
		glm::vec3 EmissionColor;
		float EmissionStrength;
		float Metallic;
		float padding[3]; // Padding to ensure 16-byte alignment
	};

	Shader m_Shader = Shader("../src/shaders/vertex.glsl", "../src/shaders/fragment.glsl");
	uint32_t m_TriangleSize, m_MeshSize;
	uint32_t m_QuadVAO, m_QuadVBO;
	uint32_t m_Framebuffer, m_FramebufferTexture, m_RenderBuffer;
	uint32_t m_TriangleSSBO, m_MeshSSBO, m_MaterialSSBO;
};
