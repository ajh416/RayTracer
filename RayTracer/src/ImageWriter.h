#pragma once

#include "RayTracer.h"

#include "Image.h"

#include <fstream>
#include <string>

enum class ImageType
{
	PNG = 0, JPG, Unknown
};

class ImageWriter
{
public:
	static bool Write(const int width, const int height, const std::string& filename = "image", const uint8_t* data = nullptr);

	static bool Write(Image& img, const std::string& filename = "image.png");
};