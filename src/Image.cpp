#include "Image.h"

Image::Image(unsigned width, unsigned height, unsigned channels) : Width(width), Height(height), Channels(channels), Size(width*height)
{
	Data = new uint32_t[Size];
}

Image::~Image()
{
	delete[] Data;
}

void Image::Resize(unsigned width, unsigned height)
{
	if (width == Width && height == Height)
		return;

	Width = width;
	Height = height;
	Size = width * height;
	delete[] Data;
	Data = new uint32_t[Size];
}