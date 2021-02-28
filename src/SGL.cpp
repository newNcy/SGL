
#include "SGL.h"
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "debug.h"


bool initSGL()
{
	return SDL_Init(SDL_INIT_EVERYTHING);
}
void releaseSGL()
{
	SDL_Quit();
}

bool SGLContext::init(const char * winTitle, int x, int y, int width, int height, int winflags)
{
	this->width = width;
	this->height = height;

	window = SDL_CreateWindow(winTitle, x, y, width, height, winflags);
	if (!winflags) {
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		destroy();
		return false;
	}

	renderBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!renderBuffer) {
		destroy();
		return false;
	}

	if (SDL_SetRelativeMouseMode((SDL_bool)true)) {
		return 0;
	}

	rgba8888 = new uint8_t[width*height*4];

	return true;
}

void SGLContext::swapBuffer(const FrameBuffer & buffer)
{
	if (renderer && renderBuffer) {
		//update pixels
		for (int y = 0; y < buffer.getHeight(); ++ y) {
			for (int x = 0; x < buffer.getWidth(); ++ x) {
				int start = (y * buffer.getWidth() + x)*4;
				auto & pixel = buffer.getPixel(x, y);

				rgba8888[start] = 255*pixel.b;
				rgba8888[start+1] = 255*pixel.g;
				rgba8888[start+2] = 255*pixel.r;
				rgba8888[start+3] = 255*pixel.a;
			}
		}
		SDL_UpdateTexture(renderBuffer, nullptr, rgba8888, buffer.getWidth()*4);
		//SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, renderBuffer, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}
}

void SGLContext::destroy()
{
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
	if (rgba8888) {
		delete [] rgba8888;
		rgba8888 = nullptr;
	}
}

