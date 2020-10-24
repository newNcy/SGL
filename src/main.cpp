#include <cstdio>
#include <SDL.h>
#include "FrameBuffer.h"
#include <math.h>
#include "SGL.h"
#include "maths.h"
#include <algorithm>


void printvec2(const Vec2i & v)
{
	printf("{%d,%d}\n", v.x, v.y);
}

int main(int argc, char * argv[])
{
	initSGL();

	SGLContext sgl;
	sgl.init("soft render graphics library", WINPOS_CENTER, WINPOS_CENTER, 800, 800);

	SGLPineline pipeline;
	pipeline.makeFrameBuffer(800, 800);

	ScreenPoint a = 
	{
		{400,100},
		{1, 0, 0}
	};
	
	ScreenPoint b = 
	{
		{700, 300},
		{0, 1, 0}
	};
	
	ScreenPoint c = 
	{
		{100, 500},
		{0, 0, 1}
	};
	
	ScreenPoint d = 
	{
		{100, 100},
		{0, 0, 1}
	};

	pipeline.drawScreenLine(a, d);

	pipeline.drawScreenTriangle(a, b, c);

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
			sgl.swapBuffer(pipeline.getCurrentFrameBuffer());
		}
	}
	sgl.destroy();
	releaseSGL();
	return 0;
}
