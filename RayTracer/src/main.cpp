#include "RayTracer.h"

#include "ImageWriter.h"
#include "Renderer.h"
#include "Ray.h"
#include "Vector.h"
#include "Scene.h"
#include "Sphere.h"

// TODO: CREATE MORE OBJECTS IN THE SCENE
// TODO: BOUNDING BOXES AND RAY HIT DETECTION

int main()
{
	PROFILE_FUNCTION()

	Logger::Init();

	constexpr int image_width = 200;
	constexpr float aspect_ratio = 16.0f / 9.0f;
	constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
	
	Renderer renderer;
	renderer.SetSettings({ .NumberOfSamples = 25, .NumberOfBounces = 5 });

	Image img(image_width, image_height, 4);
	Camera cam(image_width, aspect_ratio, {0, 0.3, 0});
	Scene scene;

	// For shapes, Z must be negative to be "seen" by the camera
	// From RaytracingInOneWeekend, we use a right-handed coordinate system
	scene.shapes.push_back(new Sphere({ 0.0, 1.5, -1.0 }, 0.5, { { 0.8, 0.5, 0.9 }, 0.2, 0.0 }));
	scene.shapes.push_back(new Sphere({ 0.0, 0.1, -1.0 }, 0.4, { { 0.8, 0.8, 0.4 }, 0.0, 0.0 }));
	scene.shapes.push_back(new Sphere({ 0.0, -10.0, -1.0 }, 9.0, { { 0.3, 0.3, 0.8 }, 1.0, 0.0 }));

	renderer.SetImage(img);
	renderer.Render(scene, cam);

	ASSERT(ImageWriter::Write(ImageType::PNG, img), "Image write failed!");
}
