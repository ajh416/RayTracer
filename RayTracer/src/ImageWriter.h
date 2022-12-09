#pragma once

#include "RayTracer.h"

#include "Image.h"

#include <fstream>
#include <string>

enum class ImageType
{
	PNG = 0, JPG
};

class ImageWriter
{
public:
	static bool Write(ImageType type, const int width, const int height, const std::string& filenameNoExt = "image", const uint8_t* data = nullptr);

	static bool Write(ImageType type, Image img, const std::string& filenameNoExt = "image");
};