#include <cstdio>
#include <SDL.h>
#include "FrameBuffer.h"
#include "math.h"
#include "SGL.h"
#include <algorithm>


void drawLine(FrameBuffer & frame, int x1, int y1, int x2, int y2, float r, float g, float b)
{
	//竖直线
	if (x1 == x2) {
		int step = y2-y1;
		int len = abs(y2-y1);
		step /= len;
		for (int y = y1; y != y2; y += step) {
			frame.setPixel(x1, y, r, g, b);
		}
	} else if (y1 == y2) {
		int step = x2-x1;
		int len = abs(x2-x1);
		step /= len;
		for (int x = x1; x != x2; x += step) {
			frame.setPixel(x, y1, r, g, b);
		}
	} else {
		float k = float(y2-y1)/(x2-x1);	
		int step = x2-x1;
		int len = abs(x2-x1);
		step /= len;
		for (int x = x1; x != x2; x += step) {
			frame.setPixel(x, y1+((x-x1)*k), r, g, b);
		}
	}
}

void drawLine2(FrameBuffer & frame, int x1, int y1, int x2, int y2, float r, float g, float b)
{
	int dy = y2 - y1;
	int dx = x2 - x1;
	bool steep = abs(dy) > abs(dx);
	if (steep) {
		std::swap(dx, dy);
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	float k = abs(float(dy) / dx);
	float err = 0;
	int ystep = y2 > y1? 1 : -1;

	while (x1 != x2) {
		if (steep) {
			frame.setPixel(y1, x1, r, g, b);
		}else {
			frame.setPixel(x1, y1, r, g, b);
		}
		err += k;
		x1 ++;
		if (err > 0.5) {
			y1 += ystep;
			err -= 1;
		}
	}
}

int main(int argc, char * argv[])
{
	initSGL();

	SGLContext sgl;
	sgl.init("soft render graphics library", WINPOS_CENTER, WINPOS_CENTER, 800, 600);

	FrameBuffer frame(800, 600);
	frame.setPixel(400, 300, 1, 1, 1);

	drawLine2(frame, 400, 100, 700, 300, 1, 0, 0);
	drawLine2(frame, 700, 300, 100, 500, 0, 0, 1);
	drawLine2(frame, 100, 500, 400, 100, 0, 1, 0);

	drawLine2(frame, 100, 100, 95, 500, 0, 1, 0);
	drawLine2(frame, 100, 100, 700, 95, 0, 1, 0);

	bool quit = false;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			//handle events
			if (event.type == SDL_QUIT) {
				quit = true;
			}

			//do things			
			//rendering
			sgl.swapBuffer(frame);
		}
	}
	sgl.destroy();
	releaseSGL();
	return 0;
}
