#pragma once
#include <SDL.h>
#include "FrameBuffer.h"
#include "maths.h"

#define WINPOS_CENTER SDL_WINDOWPOS_CENTERED
#define WIN_FULLSCREEN SDL_WINDOW_FULLSCREEN
#define WIN_RESIZABLE SDL_WINDOW_RESIZABLE 



bool initSGL();
void releaseSGL();


struct Vertex
{
	Vec3f position;	//位置
	Vec3f color;	//颜色
	Vec2f uv;		//纹理坐标
	Vec2f norm;		//法向量
};

struct BoundingBox
{
	Vec2i min;
	Vec2i max;
};

struct ScreenPoint
{
	Vec2i position;
	Vec3f color;
};

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

class SGLPineline
{
	public:
		void makeFrameBuffer(int width, int height);
		const FrameBuffer & getCurrentFrameBuffer() const;
		void drawScreenLine(const ScreenPoint & a, const ScreenPoint & b);
		void drawScreenTriangle(const ScreenPoint & a, const ScreenPoint & b, const ScreenPoint & c);
	private:
		FrameBuffer * currentFrame = nullptr;
};
