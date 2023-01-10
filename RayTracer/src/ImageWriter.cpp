#include "ImageWriter.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION // Needed to allow usage of write functions
#include "stb_image/stb_image_write.h"

bool ImageWriter::Write(const int width, const int height, const std::string& filename, const uint8_t* data)
{
	const char* res = strrchr(filename.c_str(), '.');
	ImageType type;
	if (strcmp(res, ".png"))
		type = ImageType::PNG;
	else if (strcmp(res, ".jpg"))
		type = ImageType::JPG;
	else
		type = ImageType::Unknown;

	switch (type)
	{
	case ImageType::PNG:
		return stbi_write_png(filename.c_str(), width, height, 4, data, width * 4);
		break;
	case ImageType::JPG:
		return stbi_write_jpg(filename.c_str(), width, height, 3, data, 100);
		break;
	}

	return true;
}

bool ImageWriter::Write(Image& img, const std::string& filename)
{
	PROFILE_FUNCTION();

	stbi_flip_vertically_on_write(true);
	
	const char* res = strrchr(filename.c_str(), '.');
	ImageType type;
	if (strcmp(res, ".png"))
		type = ImageType::PNG;
	else if (strcmp(res, ".jpg"))
		type = ImageType::JPG;
	else
		type = ImageType::Unknown;

	switch (type)
	{
	case ImageType::PNG:
	{
		uint32_t* cpyData = (uint32_t*)malloc(img.Size * sizeof(uint32_t));
		memcpy(cpyData, img.Data, sizeof(img.Size * sizeof(uint32_t)));
		bool success = stbi_write_png(filename.c_str(), img.Width, img.Height, img.Channels, img.Data, img.Width * 4);
		img.Data = (uint32_t*)malloc(img.Size * sizeof(uint32_t));
		memcpy(img.Data, cpyData, sizeof(img.Size * sizeof(uint32_t)));
		free(cpyData);
		return success;
	}
	case ImageType::JPG:
	{
		uint32_t* cpyData = (uint32_t*)malloc(img.Size * sizeof(uint32_t));
		memcpy(cpyData, img.Data, sizeof(img.Size * sizeof(uint32_t)));
		bool success = stbi_write_jpg(filename.c_str(), img.Width, img.Height, img.Channels, img.Data, 100);
		img.Data = (uint32_t*)malloc(img.Size * sizeof(uint32_t));
		memcpy(img.Data, cpyData, sizeof(img.Size * sizeof(uint32_t)));
		free(cpyData);
		return success;
	}
	}

	return true;
}