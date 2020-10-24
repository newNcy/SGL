#pragma once

#include <stdint.h>
#include "maths.h"


class FrameBuffer
{
	public:
		using Pixel = Vec3f;
		FrameBuffer(int width, int height);
		~FrameBuffer();

		void assertXY(int x, int y) const;
		void setPixel(int x, int y, float r, float g, float b);
		void setPixel(int x, int y, const Pixel & pixel);
		const Pixel & getPixel(int x, int y) const ;
		const void * getBuffer() const { return pixels; }

		int getWidth() const { return width; }
		int getHeight() const { return height; }
	private:	
		int width = 0;
		int height = 0;	
		Pixel * pixels = nullptr;
		float * depth = nullptr;
};

