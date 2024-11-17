#include "Image.h"

Image::Image(unsigned width, unsigned height, unsigned channels) : Width(width), Height(height), Channels(channels), Size(width*height)
{
	Data = new uint32_t[Size];
}

Image::~Image()
{
	delete[] Data;
}