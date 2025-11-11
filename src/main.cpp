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

#include <imgui.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO: TRIANGLE MESHES AND PERHAPS GPU

void DisplayObjects(Scene& scene);
void DisplayMaterials(Scene& scene);

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
	scene.Objects.push_back(new Mesh("../../../ico_sphere.wavefront", 0));

	scene.Objects.push_back(new Plane({0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, 1));

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

	renderer.SetImage(img);
	renderer.Render(scene, cam);

	// window must be created before using any OpenGL
	Window window(2000, 1100, "RayTracer");
	// initialize input system
	Input::Init(window.GetWindow());
	// create texture with image data
	// ideally we'll use a framebuffer or texture when rendering using gpu, but this works for cpu
	Texture tex(img.Width, img.Height, (uint8_t *)img.Data);
	double frametime = 0.0;
	double lastTime = glfwGetTime();

	while (!window.ShouldClose()) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.Render(scene, cam);
		tex.SetData((uint8_t*)img.Data);

		window.BeginImGui();

		ImGui::Begin("Settings");
		static int samples = 1;
		ImGui::SliderInt("Samples", &samples, 1, 100);
		static int bounces = 5;
		ImGui::SliderInt("Bounces", &bounces, 0, 100);
		static bool accumulate = true;
		ImGui::Checkbox("Accumulate", &accumulate);
		ImGui::Text("Frame (accumulation): %d", renderer.GetFrameIndex());
		ImGui::Text("Frame Time: %.3fms", frametime * 1000);
		ImGui::End();

		ImGui::Begin("Debug");
		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", cam.GetPosition().x, cam.GetPosition().y, cam.GetPosition().z);
		ImGui::End();

		renderer.SetSettings({ .NumberOfSamples = samples, .NumberOfBounces = bounces, .Accumulate = accumulate });

		DisplayObjects(scene);
		DisplayMaterials(scene);

		ImGui::Begin("Image");
		ImGui::Image((uintptr_t)tex.GetRendererID(), ImVec2((float)img.Width, (float)img.Height), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		window.EndImGui();
		window.Update();
		if (cam.Update()) {
			renderer.ResetFrameIndex();
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
