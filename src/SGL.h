#pragma once
#include <SDL.h>
#include "FrameBuffer.h"

#define WINPOS_CENTER SDL_WINDOWPOS_CENTERED
#define WIN_RESIZABLE SDL_WINDOW_RESIZABLE 



bool initSGL();
void releaseSGL();

class SGLContext
{
	public:
		using Event = SDL_Event;
		bool init(const char * winTitle, int x, int y, int width, int height, int winflags = WIN_RESIZABLE);
		void swapBuffer(const FrameBuffer & buffer);
		void destroy();
	private:
		int width = 0;
		int height = 0;
		SDL_Window * window = nullptr;
		SDL_Renderer * renderer = nullptr;
		SDL_Texture * renderBuffer = nullptr;
		uint8_t * rgb888 = nullptr;
};

