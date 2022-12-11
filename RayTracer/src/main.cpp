#include "RayTracer.h"

#include "ImageWriter.h"
#include "Renderer.h"
#include "Ray.h"
#include "Vector.h"

// TODO: CREATE MORE OBJECTS IN THE SCENE
// TODO: BOUNDING BOXES?

int main()
{
	PROFILE_FUNCTION()

	Logger::Init();

	constexpr int image_width = 3840;
	constexpr float aspect_ratio = 16.0f / 9.0f;
	constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

	Image img(image_width, image_height, 4);
	Camera cam(image_width, aspect_ratio);

	Renderer::Render(img, cam);

	ASSERT(ImageWriter::Write(ImageType::PNG, img), "Image write failed!");
}
