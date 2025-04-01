#include "RayTracer.h"

#include "ImageWriter.h"
#include "Objects/Box.h"
#include "Objects/Plane.h"
#include "Objects/Sphere.h"
#include "Objects/Triangle.h"
#include "Objects/Mesh.h"
#include "Renderer.h"
#include "Scene.h"

#include "Input.h"

#include "OpenGL/Texture.h"
#include "OpenGL/Window.h"
#include "OpenGL/Shader.h"

#include <imgui.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO: TRIANGLE MESHES AND PERHAPS GPU

// GPU STUFF NEEDS TO BE REFACTORED INTO SEPARATE FILE
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

void DisplayObjects(Scene& scene);
void DisplayMaterials(Scene& scene);
void SetupFullscreenQuad();
void SetupFramebuffer();

uint32_t quadVAO, quadVBO;
uint32_t fbo, fboTexture, rbo;
uint32_t ssboTriangles, ssboMeshes, ssboMaterials;

int main() {
	constexpr int image_width = 1280;
	constexpr float aspect_ratio = 16.0f / 9.0f;
	constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

	Renderer renderer;

	// Setting accumulate to true enables path tracing, which takes much longer to complete
	renderer.SetSettings({.NumberOfSamples = 1, .NumberOfBounces = 5, .Accumulate = true});

	// Create image scene and camera
	Image img(image_width, image_height, 4);
	Camera cam(image_width, aspect_ratio, {0, 1.25, 0});
	Scene scene;

	// ====================================================================
	// For Objects, Z must be negative to be "seen" by the camera
	// From RaytracingInOneWeekend, we use a right-handed coordinate system
	// ====================================================================

	// scene.Objects.push_back(new Sphere({ -3.0f, 7.0f, -10.0f }, 5.0f, 0));
	scene.Objects.push_back(new Mesh("../ico_sphere.wavefront", 0));
	scene.Objects.push_back(new Mesh("../monkey.obj", 1));
	dynamic_cast<Mesh*>(scene.Objects.back())->MoveTo({2.0f, 0.0f, -2.0f});

	// Vector of materials accessed using indices
	// look at this fancy syntax!
	// is it good syntax? not sure.
	scene.Materials.push_back(Material({.Albedo = {0.0f, 0.0f, 0.0f},
				.Roughness = 0.1f,
				.Metallic = 0.0f,
				.EmissionColor = glm::vec3(0.9f, 0.4f, 0.8f),
				.EmissionStrength = 1.0f}));

	scene.Materials.push_back(Material({.Albedo = {0.2f, 0.8f, 0.2f},
				.Roughness = 0.1f,
				.Metallic = 0.0f,
				.EmissionColor = glm::vec3(0.0f, 0.0f, 0.0f),
				.EmissionStrength = 0.0f}));

	//renderer.SetImage(img);
	//renderer.Render(scene, cam);

	// window must be created before using any OpenGL
	Window window(2000, 1100, "RayTracer");
	// initialize input system
	Input::Init(window.GetWindow());
	// create texture with image data
	// ideally we'll use a framebuffer or texture when rendering using gpu, but this works for cpu
	//Texture tex(img.Width, img.Height, (uint8_t *)img.Data);

	SetupFullscreenQuad();
	SetupFramebuffer();
	Shader shader("../src/shaders/vertex.glsl", "../src/shaders/fragment.glsl");
	double frametime = 0.0;
	double lastTime = glfwGetTime();

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

	shader.Bind();

	// Debug output
	std::cout << "Triangle count: " << triangles.size() << std::endl;
	std::cout << "Mesh count: " << meshes.size() << std::endl;

	// Upload triangles - Clear any previous buffer
	glDeleteBuffers(1, &ssboTriangles);
	glGenBuffers(1, &ssboTriangles);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTriangles);
	glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(TriangleGPU), triangles.data(), GL_STATIC_DRAW);
	// This is critical - bind to index 0
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboTriangles);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	// Upload meshes - Clear any previous buffer
	glDeleteBuffers(1, &ssboMeshes);
	glGenBuffers(1, &ssboMeshes);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMeshes);
	glBufferData(GL_SHADER_STORAGE_BUFFER, meshes.size() * sizeof(MeshGPU), meshes.data(), GL_STATIC_DRAW);
	// This is critical - bind to index 1
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboMeshes);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	// Upload materials - Create buffer for materials
	glDeleteBuffers(1, &ssboMaterials);
	glGenBuffers(1, &ssboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(MaterialGPU), materials.data(), GL_DYNAMIC_DRAW);
	// Bind to index 2
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

	shader.Unbind();

	glViewport(0, 0, img.Width, img.Height);
	while (!window.ShouldClose()) {
		static int samples = 1;
		static int bounces = 3;
		static bool accumulate = true;

		//renderer.Render(scene, cam);
		//tex.SetData((uint8_t*)img.Data);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Bind();
		// vertex uniforms

		// fragment uniforms
		shader.SetUniformMat4f("ViewMatrix", cam.GetInverseView());
		shader.SetUniformMat4f("ProjectionMatrix", cam.GetInverseProjection());
		shader.SetUniform3f("CameraPosition", cam.GetPosition());
		shader.SetUniform1i("NumberOfSamples", samples);
		shader.SetUniform1i("NumberOfBounces", bounces);
		shader.SetUniform1f("Time", static_cast<float>(glfwGetTime())); // Add time for random seed
		shader.SetUniform1i("TriangleCount", triangles.size());
		shader.SetUniform1i("MeshCount", meshes.size());

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
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMaterials);
		glBufferData(GL_SHADER_STORAGE_BUFFER, updatedMaterials.size() * sizeof(MaterialGPU), updatedMaterials.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboMaterials);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboTriangles);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboMeshes);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboMaterials);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		shader.Unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		window.BeginImGui();

		ImGui::Begin("Settings");
		ImGui::SliderInt("Samples", &samples, 1, 100);
		ImGui::SliderInt("Bounces", &bounces, 0, 100);
		ImGui::Checkbox("Accumulate", &accumulate);
		ImGui::Text("Frame (accumulation): %d", renderer.GetFrameIndex());
		ImGui::Text("Frame Time: %.3fms", frametime * 1000);
		ImGui::End();

		ImGui::Begin("Debug");
		auto pos = cam.GetPosition();
		auto dir = cam.GetForwardDirection();
		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
		ImGui::Text("Camera Direction: (%.2f, %.2f, %.2f)", dir.x, dir.y, dir.z);
		ImGui::End();

		renderer.SetSettings({ .NumberOfSamples = samples, .NumberOfBounces = bounces, .Accumulate = accumulate });

		DisplayObjects(scene);
		DisplayMaterials(scene);

		ImGui::Begin("Image");
		ImGui::Image((uintptr_t)fboTexture, ImVec2((float)img.Width, (float)img.Height), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		window.EndImGui();
		window.Update();
		if (cam.Update()) {
			//renderer.ResetFrameIndex();
		}
		frametime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
	}

	ASSERT(ImageWriter::Write(img), "Image write failed!")
}

void DisplayObjects(Scene& scene) {
	ImGui::Begin("Objects");
	int i = 0;
	for (auto& object : scene.Objects) {
		char label[32];
		sprintf(label, "Object %d", i);
		if (ImGui::BeginMenu(label)) {
			ImGui::SliderFloat3("Position", &object->Origin.x, -10.0f, 10.0f);
			if (object->GetType() == ObjectType::Sphere) {
				auto sphere = dynamic_cast<Sphere*>(object);
				ImGui::SliderFloat("Radius", &sphere->Radius, 0.0f, 10.0f);
			}
			if (object->GetType() == ObjectType::Triangle) {
				auto triangle = dynamic_cast<Triangle*>(object);
				ImGui::SliderFloat3("Vertex 1", &triangle->Vertices[0].x, -10.0f, 10.0f);
				ImGui::SliderFloat3("Vertex 2", &triangle->Vertices[1].x, -10.0f, 10.0f);
				ImGui::SliderFloat3("Vertex 3", &triangle->Vertices[2].x, -10.0f, 10.0f);
			}
			if (object->GetType() == ObjectType::Mesh) {
				auto mesh = dynamic_cast<Mesh*>(object);
				static glm::vec3 newOrigin;
				ImGui::SliderFloat3("Adjust Position", &newOrigin.x, -100.0f, 100.0f);
				if (ImGui::Button("Update Mesh Position")) {
					mesh->MoveTo(newOrigin);
				}
				int j = 0;
				for (auto& tri : mesh->MeshTriangles) {
					ImGui::PushID(j);
					char label2[32];
					sprintf(label2, "Triangle %d", j++);
					ImGui::SeparatorText(label2);
					ImGui::SliderFloat3("Vertex 1", &tri.Vertices[0].x, -10.0f, 10.0f);
					ImGui::SliderFloat3("Vertex 2", &tri.Vertices[1].x, -10.0f, 10.0f);
					ImGui::SliderFloat3("Vertex 3", &tri.Vertices[2].x, -10.0f, 10.0f);
					ImGui::PopID();
				}
			}
			ImGui::EndMenu();
		}
		i++;
	}
	ImGui::End();
}

void DisplayMaterials(Scene& scene) {
	ImGui::Begin("Materials");
	int i = 0;
	for (auto& material : scene.Materials) {
		char label[32];
		sprintf(label, "Material %d", i);
		if (ImGui::BeginMenu(label)) {
			ImGui::ColorEdit3("Albedo", &material.Albedo.x);
			ImGui::SliderFloat("Roughness", &material.Roughness, 0.0f, 1.0f);
			ImGui::SliderFloat("Metallic", &material.Metallic, 0.0f, 1.0f);
			ImGui::ColorEdit3("Emission Color", &material.EmissionColor.x);
			ImGui::SliderFloat("Emission Strength", &material.EmissionStrength, 0.0f, 1.0f);
			ImGui::EndMenu();
		}
		i++;
	}
	ImGui::End();
}

void SetupFullscreenQuad() {
	float quadVertices[] = {
		// Positions   // Texture Coords
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		-1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
		1.0f,  1.0f,  1.0f, 1.0f  // Top-right
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SetupFramebuffer() {
	int screenWidth = 1280;
	int screenHeight = screenWidth / (16.0f / 9.0f);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create texture to render into
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

	// Create Renderbuffer Object (RBO) for depth/stencil
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
