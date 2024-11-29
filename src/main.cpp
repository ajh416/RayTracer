#include "RayTracer.h"

#include "ImageWriter.h"
#include "Objects/Box.h"
#include "Objects/Plane.h"
#include "Objects/Sphere.h"
#include "Objects/Triangle.h"
#include "Renderer.h"
#include "Scene.h"

#include "OpenGL/Texture.h"
#include "OpenGL/Window.h"

#include <imgui.h>

// TODO: TRIANGLE MESHES AND PERHAPS GPU

int main() {
	Logger::Init();

	constexpr int image_width = 480;
	constexpr float aspect_ratio = 16.0f / 9.0f;
	constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

	Renderer renderer;

	// Setting accumulate to true enables path tracing, which takes much longer to complete
	renderer.SetSettings({.NumberOfSamples = 1, .NumberOfBounces = 5, .Accumulate = true, .AccumulateMax = 10});

	// Create image scene and camera
	Image img(image_width, image_height, 4);
	Camera cam(image_width, aspect_ratio, {0, 1.25, 0});
	Scene scene;

	// ====================================================================
	// For Objects, Z must be negative to be "seen" by the camera
	// From RaytracingInOneWeekend, we use a right-handed coordinate system
	// ====================================================================

	// scene.Objects.push_back(new Sphere({ -3.0f, 7.0f, -10.0f }, 5.0f, 0));

	scene.Objects.push_back(new Sphere({3.0f, 2.0f, -1.0f}, 1.0f, 1));

	scene.Objects.push_back(new Sphere({0.0f, -19.0f, -1.0f}, 20.0f, 2));

	scene.Objects.push_back(new Plane({0.0f, -1.0f, -10.0f}, {0.0f, 0.0f, -1.0f}, 3));

	scene.Objects.push_back(new Plane({0.0f, 4.0f, 10.0f}, {0.0f, 0.0f, 1.0f}, 3));

	scene.Objects.push_back(new Triangle({Vec3f(-1.0f, 1.0f, -1.0f), Vec3f(1.0f, 1.0f, -1.0f), Vec3f(0, 2, -1)}));

	scene.Objects.push_back(new Box({{2.0f, 1.0f, -1.0f}, {3.0f, 2.0f, -2.0f}}, 0));

	// Vector of materials accessed using indices
	// look at this fancy syntax!
	scene.Materials.push_back(Material({.Albedo = {0.0f, 0.0f, 0.0f},
				.Roughness = 0.1f,
				.Metallic = 0.0f,
				.EmissionColor = Vec3f(0.9f, 0.4f, 0.8f),
				.EmissionStrength = 1.0f}));
	scene.Materials.push_back(Material({{0.6f, 0.6f, 0.8f}, 0.1f, 0.0f, Vec3f(0.0f), 0.0f}));
	scene.Materials.push_back(Material({{0.6f, 0.6f, 0.6f}, 0.05f, 0.0f, Vec3f(0.0f), 0.0f}));
	scene.Materials.push_back(Material({{0.6f, 0.6f, 0.6f}, 0.1f, 0.0f, Vec3f(0.3f, 0.3f, 0.3f), 1.0f}));

	renderer.SetImage(img);
	renderer.Render(scene, cam);

	bool end = false;
	// window must be created before using any OpenGL
	Window window(1280, 720, "RayTracer");
	Texture tex(img.Width, img.Height, (uint8_t *)img.Data);

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
		ImGui::SliderInt("Bounces", &bounces, 1, 100);
		static bool accumulate = true;
		ImGui::Checkbox("Accumulate", &accumulate);
		static int accumulateMax = 10;
		ImGui::SliderInt("Accumulate Max", &accumulateMax, 1, 100);
		if (ImGui::Button("Save")) {
			renderer.SetSettings({.NumberOfSamples = samples,
					.NumberOfBounces = bounces,
					.Accumulate = accumulate,
					.AccumulateMax = accumulateMax});
		}
		ImGui::End();
		ImGui::Begin("Image");
		ImGui::Image((uintptr_t)tex.GetRendererID(), ImVec2(img.Width, img.Height), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
		window.EndImGui();
		window.Update();
	}

	ASSERT(ImageWriter::Write(img), "Image write failed!")
}
