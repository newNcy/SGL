#include <cstdio>
#include <SDL.h>
#include <algorithm>
#include "FrameBuffer.h"
#include <math.h>
#include "SGL.h"
#include "maths.h"
#include "debug.h"


int main(int argc, char * argv[])
{
	initSGL();

	SGLContext sgl;
	sgl.init("wai bi ba bo", WINPOS_CENTER, WINPOS_CENTER, 800, 800);
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

	//pipeline.drawScreenLine(a, d);

	//pipeline.drawScreenTriangle(a, b, c);


	Vertex cube[] = 
	{
		{{-1.f, 1.f, -1.f}, {.color = {1.f,0.f,0.f}}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f, -1.f}, {.color = {1.f,1.f,0.f}}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f, -1.f}, {.color = {1.f,0.f,1.f}}, {0.f,0.f,0.f}},
		{{-1.f,-1.f, -1.f}, {.color = {1.f,0.f,0.f}}, {0.f,0.f,0.f}},

		{{-1.f, 1.f,  1.f}, {.color = {0.f,1.f,0.f}}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f,  1.f}, {.color = {0.f,1.f,0.f}}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {.color = {0.f,1.f,0.f}}, {0.f,0.f,0.f}},
		{{-1.f,-1.f,  1.f}, {.color = {0.f,1.f,0.f}}, {0.f,0.f,0.f}}
	};

	unsigned int indies[] = 
	{
		0, 1, 2, 
		0, 3, 2,

		4, 5, 6,
		4, 7, 6,

		4, 0, 3,
		4, 7, 3,

		1, 5, 6,
		1, 2, 5,

		4, 5, 1,
		4, 0, 1,
		
		3, 2, 6,
		3, 7, 6
	};

	class NormalShader : public SGLShader
	{
		const float pi = 3.1415926;
		private:
			Mat4f model, view, projection;
		public:
			NormalShader()
			{
				model = 
				{
					{1, 0, 0, 0},
					{0, 1.f, 0, 0},
					{0, 0, 1, 0}, 
					{0, 0, 0, 1}, 
				};

				Vec3f pos(0, -2, -10);
				Vec3f target(0, 0, 0);
				Vec3f up(0, 1, 0);
				view = lookat(pos, target, up);
				print(view);
				projection = perspective(60, 1.f, .1f, 1000);
			}

			Vec4f onVertex(const Vec3f & pos) override
			{
				Vec4f ret = Vec4f(pos,1) * model * view * projection;
				//print(ret, 4);
				//fflush(stdout);
				return ret;
			}
	};
	auto shader = std::make_shared<NormalShader>();
	pipeline.useShader(shader);
	pipeline.clearDepth(1.f);
	pipeline.drawElements(cube, 8, indies, 36, DrawMode::SGL_LINE);

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
