#include "FrameBuffer.h"
#include <cassert>

FrameBuffer::FrameBuffer(int width, int height):width(width), height(height)
{
	int pixelCount = width * height;
	pixels = new Pixel[pixelCount];
	depth = new float[pixelCount];
}
		

void FrameBuffer::assertXY(int x, int y) const 
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
}

void FrameBuffer::setPixel(int x, int y, float r, float g, float b)
{
	assertXY(x, y);
	int idx = y*width + x;
	pixels[idx].r = r;
	pixels[idx].g = g;
	pixels[idx].b = b;
}
		
void FrameBuffer::setPixel(int x, int y, const Pixel & pixel)
{
	assertXY(x, y);
	int idx = y*width + x;
	pixels[idx] = pixel;
}

const FrameBuffer::Pixel & FrameBuffer::getPixel(int x, int y) const 
{
	assertXY(x, y);
	int idx = y*width + x;
	return pixels[idx];
}
		
FrameBuffer::~FrameBuffer() 
{
	if (depth) {
		delete [] depth;
		depth = nullptr;
	}

	if (pixels) {
		delete [] pixels;
		pixels = nullptr;
	}
}
