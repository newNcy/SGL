
#include "SGL.h"
#include <cstdio>
#include <algorithm>
#include <vector>
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
		
void SGLPineline::clearColor(float r, float g, float b)
{
	if (!currentFrame) {
		return;
	}

	for (int y = 0; y < currentFrame->getHeight(); ++ y) {
		for (int x = 0; x < currentFrame->getWidth(); ++ x) {
			currentFrame->setPixel(x, y, r, g, b);
		}
	}
}

void SGLPineline::clearDepth(float d)
{
	if (!currentFrame) {
		return;
	}
	for (int y = 0; y < currentFrame->getHeight(); ++ y) {
		for (int x = 0; x < currentFrame->getWidth(); ++ x) {
			currentFrame->setDepth(x, y, d);
		}
	}
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

void SGLPineline::drawScreenTriangle(const ScreenPoint & a, const ScreenPoint & b, const ScreenPoint & c, bool testz)
{
	if (!currentFrame) {
		return;
	}

	auto A = a.position;
	auto B = b.position;
	auto C = c.position;

	auto bb = bbox(*currentFrame, A, B, C);
	float S = calcS(A, B, C);
	if (!S) {
		return;
	}

	for (int x = bb.min.x; x <= bb.max.x; ++ x) {
		for (int y = bb.min.y; y <= bb.max.y; ++ y) {
			Vec2i p(x, y);
			float s1 = calcS(B,p,C);
			float s2 = calcS(A,p,C);
			float s3 = calcS(A,p,B);
			
			float w1= s1/S;
			float w2 = s2/S;
			float w3 = s3/S;

			float depth = a.depth*w1+ b.depth*w2 + c.depth*w3;
			auto color = a.color*w1+ b.color*w2+ c.color*w3;
			if (s1+s2+s3 <= S) {
				float depthBuf = currentFrame->getDepth(x, y);
				if (!testz || depthBuf > depth) {
					currentFrame->setPixel(x, y, color);
					currentFrame->setDepth(x, y, depth);
				}
			}		
		}
	}
}
	

bool SGLPineline::needClip(const Vec4f & pos)
{
	for (int i = 0; i < 3; ++ i) {
		if (pos[i] < -pos.w || pos[i] > pos.w) {
			return true;
		}
	}
	return false;
}

Vec2i SGLPineline::viewPortTrans(const Vec4f & pos)
{
	float x = (pos.x + 1)*0.5;
	float y = (-pos.y + 1)*0.5;

	return Vec2i(x*currentFrame->getWidth(), y*currentFrame->getHeight());
}
		
void SGLPineline::drawArrayLine(const Vertex * verties, size_t count, DrawMode drawMode)
{
}

void SGLPineline::drawArray(const Vertex * verties, size_t count, DrawMode drawMode)
{
}

void SGLPineline::drawElements(const Vertex * verties, size_t count, unsigned int * indices, size_t indiesCount, DrawMode drawMode)
{
	if (!verties || !indices || !currentFrame || !shader) return;

	for (int i = 0; i < indiesCount/3; ++ i) {
		std::vector<ScreenPoint> screenPoints;
		for (int j = 0; j < 3; ++ j) {
			auto & vert = verties[indices[i*3 + j]];
			auto clipPoint = shader->onVertex(vert.position);
			clipPoint = clipPoint/clipPoint.w;
			print(clipPoint, 4);
			if (needClip(clipPoint)) {
				continue;
			}
			auto color = shader->onFragment(vert);
			ScreenPoint p;
			p.position = viewPortTrans(clipPoint);
			p.color = Vec3f(color.r, color.g, color.b);
			p.depth  = clipPoint.z;
			screenPoints.push_back(p);
		}
		if (screenPoints.size() == 3) {
			drawScreenTriangle(screenPoints[0], screenPoints[1], screenPoints[2], true);
		}
	}
}
