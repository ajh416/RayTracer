#include "RayTracer.h"

#include "ImageWriter.h"
#include "Renderer.h"
#include "Ray.h"
#include "Vector.h"
#include "Scene.h"
#include "Sphere.h"
#include "Box.h"

// TODO: More shapes, refactor inheritance

int main()
{
	PROFILE_FUNCTION();

	Logger::Init();

	constexpr int image_width = 1920;
	constexpr float aspect_ratio = 16.0f / 10.0f;
	constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

	Renderer renderer;

	// Setting accumulate to true enables path tracing, which takes much longer to complete
	renderer.SetSettings({ .NumberOfSamples = 1, .NumberOfBounces = 3, .Accumulate = true, .AccumulateMax = 25 });

	Image img(image_width, image_height, 4);
	Camera cam(image_width, aspect_ratio, { 0, 0, 0 });
	Scene scene;

	/**
	 * For shapes, Z must be negative to be "seen" by the camera
	 * From RaytracingInOneWeekend, we use a right-handed coordinate system
	**/
	scene.Shapes.push_back(new Sphere({ 0.0, 1.3, -1.0 }, 0.5, 0));
	scene.Shapes.push_back(new Sphere({ 0.0, 0.15, -1.0 }, 0.4, 0));
	scene.Shapes.push_back(new Box(Bounds3<Float>(Point3<Float>{ 1.0, 0.0, 1.5 }, Point3<Float>{ 2.0, 1.0, -2.0 }), 1));
	scene.Shapes.push_back(new Box(Bounds3<Float>(Point3<Float>{ -1.0, 0.0, 1.5 }, Point3<Float>{ -2.0, 1.0, -2.0 }), 1));

	//scene.Shapes.push_back(new Sphere({ 0.0, -100.0, -1.0 }, 99.6, 1));

	// Vector of materials accessed using indices
	scene.Materials.push_back(Material({ .Albedo = {0.3, 0.3, 0.8}, .Roughness = 0.1, .Metallic = 0.0 }));
	scene.Materials.push_back(Material({ {0.6, 0.6, 0.6}, 0.5, 0.0 }));

	renderer.SetImage(img);
	renderer.Render(scene, cam);

	ASSERT(ImageWriter::Write(img), "Image write failed!");
}
