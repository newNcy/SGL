#include "FrameBuffer.h"
#include <cassert>

FrameBuffer::FrameBuffer(int width, int height):width(width), height(height)
{
	int pixelCount = width * height;
	pixels = new Pixel[pixelCount];
	depth = new float[pixelCount];
}

int FrameBuffer::xy2idx(int x, int y) const 
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	return y*width + x;
}

void FrameBuffer::setPixel(int x, int y, float r, float g, float b)
{
	int idx = xy2idx(x, y);
	pixels[idx].r = r;
	pixels[idx].g = g;
	pixels[idx].b = b;
}

void FrameBuffer::setPixel(int x, int y, const Pixel & pixel)
{
	int idx = xy2idx(x, y);
	pixels[idx] = pixel;
}

void FrameBuffer::setDepth(int x, int y, float d)
{
	depth[xy2idx(x,y)] = d;
}

float FrameBuffer::getDepth(int x, int y) const
{
	int idx = xy2idx(x, y);
	return depth[idx];
}

const FrameBuffer::Pixel & FrameBuffer::getPixel(int x, int y) const 
{
	int idx = xy2idx(x, y);
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
