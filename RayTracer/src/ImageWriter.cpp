#include "ImageWriter.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION // Needed to allow usage of write functions
#include "stb_image/stb_image_write.h"

bool ImageWriter::Write(ImageType type, const int width, const int height, const std::string& filenameNoExt, const uint8_t* data)
{
	std::string ext = "";
	switch (type)
	{
	case ImageType::PNG:
		ext = ".png";
		return stbi_write_png((filenameNoExt + ext).c_str(), width, height, 4, data, width * 4);
		break;
	case ImageType::JPG:
		ext = ".jpg";
		return stbi_write_jpg((filenameNoExt + ext).c_str(), width, height, 3, data, 100);
		break;
	}

	return true;
}

bool ImageWriter::Write(ImageType type, Image& img, const std::string& filenameNoExt)
{
	PROFILE_FUNCTION();

	stbi_flip_vertically_on_write(true);
	
	std::string ext = "";
	switch (type)
	{
	case ImageType::PNG:
	{
		ext = ".png";
		uint32_t* cpyData = (uint32_t*)malloc(sizeof(img.Data));
		memcpy(cpyData, img.Data, sizeof(img.Data));
		bool success = stbi_write_png((filenameNoExt + ext).c_str(), img.Width, img.Height, img.Channels, img.Data, img.Width * 4);
		img.Data = (uint32_t*)malloc(sizeof(cpyData));
		memcpy(img.Data, cpyData, sizeof(cpyData));
		free(cpyData);
		return success;
	}
	case ImageType::JPG:
	{
		ext = ".jpg";
		bool success = stbi_write_jpg((filenameNoExt + ext).c_str(), img.Width, img.Height, img.Channels, img.Data, 100);
		if (success)
			img.Data = nullptr;
		return success;
	}
	}

	return true;
}