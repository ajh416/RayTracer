#include "RayTracer.h"

#include "ImageWriter.h"
#include "Renderer.h"
#include "Ray.h"
#include "Camera.h"
#include "Image.h"
#include "Vector.h"
#include "Point.h"
#include "Timer.h"

// color the ray based on what it hits/where it is located
Vector3<Float> RayColor(const Ray<Float>& r) {
	Vector3<Float> sphere_origin = Vector3<Float>(0, 0, 0);
	Float radius = 0.5;
	Vector3<Float> oc = r.GetOrigin() - sphere_origin; // origin of ray - origin of sphere
	auto a = Dot(r.GetDirection(), r.GetDirection()); // square the direction
	auto b = 2.0 * Dot(oc, r.GetDirection());
	auto c = Dot(oc, oc) - radius * radius;
	auto discriminant = b * b - 4.0 * a * c; // quadratic formula
	if (discriminant >= 0.0) // if we hit the sphere
	{
		double t[] = {
			(-b - sqrt(discriminant)) / (2.0 * a),
			//(-b + sqrt(discriminant)) / (2.0 * a)
		};

		for (int i = 0; i < 1; i++) // don't really need second element for most things
		{
			Vector3<Float> hit_position = r.GetOrigin() + r.GetDirection() * t[i];
			Vector3<Float> normal = hit_position - sphere_origin;
			normal = Normalize(normal);
			Vector3<Float> light_dir = Vector3<Float>(-1, 1, 1);
			light_dir = Normalize(light_dir);
			Float light_intensity = Utils::Max(Dot(normal, -light_dir), 0.0);
			return light_intensity * Vector3<Float>(1.0, 0.5, 0.4);
		}
	}

	// we only reach here if we didn't hit the sphere
	Vector3 unit_direction = Normalize(r.GetDirection()); // the unit vector (magnitude == 1) of the rays direction
	auto t = 0.5 * (unit_direction.y + 1.0); // make t 0 -> 1
	return Utils::Lerp(Vector3<Float>(1.0), Vector3<Float>(0.5, 0.7, 1.0), t); // lerp the values to white -> light blue based on y coord
}

int main()
{
	PROFILE_FUNCTION()

	Logger::Init();

	constexpr int image_width = 720;
	constexpr float aspect_ratio = 16.0f / 9.0f;
	constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

	Image img(image_width, image_height, 4);
	Camera cam(image_width, aspect_ratio);

	for (unsigned y = img.Height - 1; y > 0; y--)
	{
		for (unsigned x = 0; x < img.Width; x++)
		{
			auto lowerleft = cam.GetLowerLeftCorner();
			auto u = double(x) / (image_width - 1); // transform the x coordinate to 0 -> 1 (rather than 0 -> image_width)
			auto v = double(y) / (image_height - 1); // transform the y coordinate to 0 -> 1 (rather than 0 -> image_height)
			Ray<Float> r(cam.GetOrigin(), lowerleft + u * cam.GetHorizontal() + v * cam.GetVertical() - cam.GetOrigin());
			Vector3 color = RayColor(r);

			color = Utils::Clamp(color, Vector3(0.0), Vector3(1.0));

			img.Data[x + y * img.Width] = Utils::VectorToUInt32(color);
		}
	}

	ASSERT(ImageWriter::Write(ImageType::PNG, img), "Image write failed!");
}
