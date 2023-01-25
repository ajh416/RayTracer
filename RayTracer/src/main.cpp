#include "RayTracer.h"

#include "ImageWriter.h"
#include "Renderer.h"
#include "Ray.h"
#include "Vector.h"
#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"

// TODO: BOUNDING BOXES AND RAY HIT DETECTION

int main()
{
	PROFILE_FUNCTION();

	Logger::Init();

	constexpr int image_width = 1920;
	constexpr float aspect_ratio = 16.0f / 9.0f;
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
	scene.Shapes.push_back(new Sphere({ 0.0, -100.0, -1.0 }, 99.6, 1));
	scene.Shapes.push_back(new Plane({ 0.0, 0.0, -1.0 }, 1.0, 1.0, 1));

	// Vector of materials accessed using indices
	scene.Materials.push_back(Material({ {0.3, 0.3, 0.8}, 0.1, 0.0 }));
	scene.Materials.push_back(Material({ {0.6, 0.6, 0.6}, 0.5, 0.0 }));

	renderer.SetImage(img);
	renderer.Render(scene, cam);

	ASSERT(ImageWriter::Write(img), "Image write failed!");
}
