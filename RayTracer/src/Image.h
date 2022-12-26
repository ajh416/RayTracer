#pragma once

#include <stdint.h>

class Image
{
public:
	Image() : Width(0), Height(0), Channels(0), Size(0), Data(nullptr) {}
	Image(unsigned width, unsigned height, unsigned channels);
	~Image();

	unsigned int Width, Height, Channels; // width, height, channels
	uint64_t Size; // size of the image in terms of pixels
	uint32_t* Data; // this will be freed by stbi during write, although we circumvent with copy and free it ourselves
};