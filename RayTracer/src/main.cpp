#include "RayTracer.h"

#include "ImageWriter.h"
#include "Renderer.h"
#include "Ray.h"
#include "Vector.h"
#include "Scene.h"
#include "Sphere.h"
#include "Box.h"

// TODO: TRIANGLES AND TRIANGLE MESHES

int main()
{
	PROFILE_FUNCTION();

	Logger::Init();

	constexpr int image_width = 720;
	constexpr float aspect_ratio = 16.0f / 9.0f;
	constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

	Renderer renderer;

	// Setting accumulate to true enables path tracing, which takes much longer to complete
	renderer.SetSettings({ .NumberOfSamples = 1, .NumberOfBounces = 3, .Accumulate = true, .AccumulateMax = 25 });

	// Create image scene and camera
	Image img(image_width, image_height, 4);
	Camera cam(image_width, aspect_ratio, { 0, 2, 3 });
	Scene scene;

	// ====================================================================
	// For shapes, Z must be negative to be "seen" by the camera
	// From RaytracingInOneWeekend, we use a right-handed coordinate system
	// ====================================================================

	scene.Shapes.push_back(new Sphere({ -3.0f, 5.0f, -5.0f }, 5.0f, 0));

	scene.Shapes.push_back(new Sphere({ 3.0f, 2.0f, -1.0f }, 1.0f, 1));

	scene.Shapes.push_back(new Sphere({ 0.0f, -6.0f, -1.0f }, 6.0f, 1));
	// Floor
	//scene.Shapes.push_back(new Box(Bounds3f(Point3f{ -0.5f, 0.0f, 0.0f }, Point3f{ 0.5f, 0.2f, -1.0f }), 1));

	//scene.Shapes.push_back(new Sphere(Vec3f(0.0f, 1.5f, -1.0f), 0.25f, 2));

	// Vector of materials accessed using indices
	scene.Materials.push_back(Material({ .Albedo = {0.0f, 0.0f, 0.0f}, .Roughness = 0.1f, .Metallic = 0.0f, .EmissionColor = Vec3f(1.0f), .EmissionStrength = 1.f }));
	scene.Materials.push_back(Material({ {0.6f, 0.6f, 0.6f}, 0.25f, 0.0f, Vec3f(0.0f), 0.0f }));

	//scene.Materials.push_back(Material({ {0.6f, 0.6f, 0.6f}, 0.1f, 0.0f }));

	renderer.SetImage(img);
	renderer.Render(scene, cam);

	ASSERT(ImageWriter::Write(img), "Image write failed!");
}
