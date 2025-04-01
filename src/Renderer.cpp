#include "Renderer.h"

#include "Ray.h"

#include "Objects/Mesh.h"

#include <string.h>

#include <execution>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void Renderer::Render(const Scene &scene, Camera &cam) {
	if (m_Camera == nullptr || m_Scene == nullptr) {
		m_Camera = &cam;
		m_Scene = &scene;
	}

	if (m_RenderGPU) {
		RenderGPU(scene, cam);
		return;
	}

	if (m_RenderTexture->GetWidth() != m_Image->Width || m_RenderTexture->GetHeight() != m_Image->Height) {
		printf("Resizing render texture to %d x %d\n", m_Image->Width, m_Image->Height);
		delete m_RenderTexture;
		m_RenderTexture = new Texture(m_Image->Width, m_Image->Height);
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
        m_RenderTexture->SetData((unsigned char*)m_Image->Data);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

void Renderer::RenderGPU(const Scene& scene, const Camera& cam) {
	if (!m_GPUSetup) {
		SetupGPUBuffers(scene);
		m_GPUSetup = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_Shader.Bind();
	// vertex uniforms

	// fragment uniforms
	m_Shader.SetUniformMat4f("ViewMatrix", cam.GetInverseView());
	m_Shader.SetUniformMat4f("ProjectionMatrix", cam.GetInverseProjection());
	m_Shader.SetUniform3f("CameraPosition", cam.GetPosition());
	m_Shader.SetUniform1i("NumberOfSamples", m_Settings.NumberOfSamples);
	m_Shader.SetUniform1i("NumberOfBounces", m_Settings.NumberOfBounces);
	m_Shader.SetUniform1f("Time", static_cast<float>(glfwGetTime())); // Add time for random seed
	m_Shader.SetUniform1i("TriangleCount", m_TriangleSize);
	m_Shader.SetUniform1i("MeshCount", m_MeshSize);

	// Update materials buffer with any changes from UI
	std::vector<MaterialGPU> updatedMaterials;
	updatedMaterials.reserve(scene.Materials.size());
	for (auto& material : scene.Materials) {
		updatedMaterials.push_back({
				.Albedo = material.Albedo,
				.Roughness = material.Roughness,
				.EmissionColor = material.EmissionColor,
				.EmissionStrength = material.EmissionStrength,
				.Metallic = material.Metallic
				});
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_MaterialSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, updatedMaterials.size() * sizeof(MaterialGPU), updatedMaterials.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_MaterialSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_TriangleSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_MeshSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_MaterialSSBO);
	glBindVertexArray(m_QuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	m_Shader.Unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void Renderer::SetupGPUBuffers(const Scene &scene) {
	float quadVertices[] = {
		// Positions   // Texture Coords
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		-1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
		1.0f,  1.0f,  1.0f, 1.0f  // Top-right
	};

	glGenVertexArrays(1, &m_QuadVAO);
	glGenBuffers(1, &m_QuadVBO);
	glBindVertexArray(m_QuadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// TODO: fix
	int screenWidth = 1280;
	int screenHeight = screenWidth / (16.0f / 9.0f);
	glGenFramebuffers(1, &m_Framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

	// Create texture to render into
	glGenTextures(1, &m_FramebufferTexture);
	glBindTexture(GL_TEXTURE_2D, m_FramebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferTexture, 0);

	// Create Renderbuffer Object (RBO) for depth/stencil
	glGenRenderbuffers(1, &m_RenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<TriangleGPU> triangles;
	std::vector<MeshGPU> meshes;
	for (auto& object : scene.Objects) {
		if (object->GetType() == ObjectType::Mesh) {
			auto mesh = dynamic_cast<Mesh*>(object);
			int startTriangleIndex = triangles.size();
			int triangleCount = mesh->MeshTriangles.size();
			printf("Mesh has %d triangles\n", triangleCount);
			meshes.push_back({
					.minBounds = glm::vec4(mesh->BoundingBox.m_Box.pMin, 1.0f),
					.maxBounds = glm::vec4(mesh->BoundingBox.m_Box.pMax, 1.0f),
					.startTriangleIndex = startTriangleIndex,
					.triangleCount = triangleCount
					});
			for (auto& tri : mesh->MeshTriangles) {
				triangles.push_back({
						.v0 = glm::vec4(tri.Vertices[0], 1.0f),
						.v1 = glm::vec4(tri.Vertices[1], 1.0f),
						.v2 = glm::vec4(tri.Vertices[2], 1.0f),
						.normal = glm::vec4(tri.Normal, 0.0f),   // 0 for directions
						.materialIndex = tri.MaterialIndex
						});
			}
		}
	}

	m_TriangleSize = triangles.size();
	m_MeshSize = meshes.size();
	
	std::vector<MaterialGPU> materials;
	materials.reserve(scene.Materials.size());
	for (auto& material : scene.Materials) {
		materials.push_back({
				.Albedo = material.Albedo,
				.Roughness = material.Roughness,
				.EmissionColor = material.EmissionColor,
				.EmissionStrength = material.EmissionStrength,
				.Metallic = material.Metallic
				});
	}

	m_Shader.Bind();

	// Upload triangles - Clear any previous buffer
	glDeleteBuffers(1, &m_TriangleSSBO);
	glGenBuffers(1, &m_TriangleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,m_TriangleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(TriangleGPU), triangles.data(), GL_STATIC_DRAW);
	// This is critical - bind to index 0
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,m_TriangleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
						   // Upload meshes - Clear any previous buffer
	glDeleteBuffers(1, &m_MeshSSBO);
	glGenBuffers(1, &m_MeshSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_MeshSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, meshes.size() * sizeof(MeshGPU), meshes.data(), GL_STATIC_DRAW);
	// This is critical - bind to index 1
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_MeshSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
						   // Upload materials - Clear any previous buffer
	glDeleteBuffers(1, &m_MaterialSSBO);
	glGenBuffers(1, &m_MaterialSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_MaterialSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(MaterialGPU), materials.data(), GL_STATIC_DRAW);
	// This is critical - bind to index 2
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_MaterialSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
	
	m_Shader.Unbind();
}
