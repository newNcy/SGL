#pragma once

#include <stdint.h>
#include "maths.h"


class FrameBuffer
{
	public:
		using Pixel = Vec4f;
		FrameBuffer(int width, int height);
		~FrameBuffer();

		int xy2idx(int x, int y) const;
		void setPixel(int x, int y, float r, float g, float b, float a = 1);
		void setPixel(int x, int y, const Pixel & pixel);
		const Pixel & getPixel(int x, int y) const ;
		float getDepth(int x, int y) const;
		void setDepth(int x, int y, float d);

		int getWidth() const { return width; }
		int getHeight() const { return height; }
	private:	
		int width = 0;
		int height = 0;	
		Pixel * pixels = nullptr;
		float * depth = nullptr;
};

