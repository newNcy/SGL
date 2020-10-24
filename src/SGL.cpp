
#include "SGL.h"
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cmath>

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
	rgb888 = new uint8_t[width*height*4];
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

				rgb888[start+2] = 255*pixel.r;
				rgb888[start+1] = 255*pixel.g;
				rgb888[start+0] = 255*pixel.b;
			}
		}
		SDL_UpdateTexture(renderBuffer, nullptr, rgb888, buffer.getWidth()*4);
		SDL_RenderClear(renderer);
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
	if (rgb888) {
		delete [] rgb888;
		rgb888 = nullptr;
	}
}



void SGLPineline::makeFrameBuffer(int width, int height)
{
	//尺寸一样就重用
	if (currentFrame ) {
		if (currentFrame->getWidth() == width && currentFrame->getHeight() == height) {
			return;
		}else {
			delete currentFrame;
		}
	}
	currentFrame = new FrameBuffer(width, height);
}

const FrameBuffer & SGLPineline::getCurrentFrameBuffer() const
{
	assert(currentFrame);
	return *currentFrame;
}

void SGLPineline::drawScreenLine(const ScreenPoint & a, const ScreenPoint & b)
{
	if (!currentFrame) {
		return;
	}

	auto s = a.position;
	auto e = b.position;
	auto ca = a.color;
	auto cb = b.color;

	int dy = e.y - s.y;
	int dx = e.x - s.x;
	bool steep = abs(dy) > abs(dx);
	if (steep) {
		std::swap(dx, dy);
		std::swap(s.x, s.y);
		std::swap(e.x, e.y);
	}

	if (s.x > e.x) {
		std::swap(s.x, e.x);
		std::swap(s.y, e.y);
		std::swap(ca, cb);
	}

	float k = abs(float(dy) / dx);
	float err = 0;
	int ystep = e.y > s.y? 1 : -1;

	while (s.x != e.x) {
		auto left = float(e.x - s.x)/dx;
		Vec3f color = ca*(1-left) + cb*left;
		if (steep) {
			currentFrame->setPixel(s.y, s.x, color);
		}else {
			currentFrame->setPixel(s.x, s.y, color);
		}
		err += k;
		s.x ++;
		if (err > 0.5) {
			s.y += ystep;
			err -= 1;
		}
	}
}


BoundingBox bbox(const FrameBuffer & frame, const Vec2i & a, const Vec2i & b,const Vec2i & c)
{
	BoundingBox ret = 
	{
		{frame.getWidth()-1, frame.getHeight()-1},
		{0, 0}
	};
	std::vector<Vec2i> ps = {a, b, c};
	for (auto & p : ps) {
		for (int i = 0; i < 2; ++ i) {
			if (p[i] < ret.min[i]) {
				ret.min[i] = p[i];
			}
			if (p[i] > ret.max[i]) {
				ret.max[i] = p[i];
			}
		}
	}
	return ret;
}

float calcS(const Vec2i & a, const Vec2i & b,const Vec2i & c)
{
	auto ab = b-a;
	auto ac = c-a;
	float res = fabs(ab.x*ac.y-ab.y*ac.x);
	return res*0.5f;
}

void SGLPineline::drawScreenTriangle(const ScreenPoint & a, const ScreenPoint & b, const ScreenPoint & c)
{
	if (!currentFrame) {
		return;
	}

	auto A = a.position;
	auto B = b.position;
	auto C = c.position;

	auto bb = bbox(*currentFrame, A, B, C);
	float S = calcS(A, B, C);

	for (int x = bb.min.x+1; x < bb.max.x; ++ x) {
		for (int y = bb.min.y+1; y < bb.max.y; ++ y) {
			Vec2i p(x, y);
			float s1 = calcS(B,p,C);
			float s2 = calcS(A,p,C);
			float s3 = calcS(A,p,B);

			auto color = a.color*s1/S + b.color*s2/S + c.color*s3/S;
			if (s1+s2+s3 <= S) {
				currentFrame->setPixel(x, y, color);
			}		
		}
	}
}
