#include <cstdio>
#include <SDL.h>
#include <algorithm>
#include "FrameBuffer.h"
#include <math.h>
#include "SGL.h"
#include "maths.h"
#include "debug.h"
//#include <glm/glm.hpp>


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
		{{-1.f, 1.f, -1.f}, {.color = {1.f,0.f,1.f}}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f, -1.f}, {.color = {1.f,1.f,0.f}}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f, -1.f}, {.color = {1.f,0.f,0.f}}, {0.f,0.f,0.f}},
		{{-1.f,-1.f, -1.f}, {.color = {1.f,0.f,0.f}}, {0.f,0.f,0.f}},

		{{-1.f, 1.f,  1.f}, {.color = {1.f,1.f,1.f}}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f,  1.f}, {.color = {0.f,1.f,0.f}}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {.color = {0.f,1.f,1.f}}, {0.f,0.f,0.f}},
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
		1, 2, 6,

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
			void setModel(const Mat4f & m) 
			{
				model = m;
			}

			void setCamera(const Mat4f & c)
			{
				view = c;
			}

			NormalShader()
			{
				//print(view);
				projection = perspective(90, 1.f, .1f, 1000);
			}

			Vec4f onVertex(const Vec3f & pos) override
			{
				Vec4f ret = Vec4f(pos,1) * model * view * projection;
				return ret;
			}
	};

	auto shader = std::make_shared<NormalShader>();
	pipeline.useShader(shader);
	
	//第一个
	Vec3f campos(-20, 20, -20);
	Vec3f up(0, 1, 0);
	Vec3f focus(0, 0, 0);
	auto view = lookat(campos, focus, up);

	shader->setCamera(view);

	bool quit = false;
	auto mode = DrawMode::SGL_TRIANGLE;

	float xr = 0, yr = 0, zr = 0;

	if (SDL_CaptureMouse((SDL_bool)true)) {
		return 0;
	}
	while (!quit) {
		pipeline.clearColor(.1f, .1f, .1f);
		pipeline.clearDepth(1.f);

		/*
		auto model = rotate(15,15,0) * moveto(-5, 0, 0) ;
		shader->setModel(model);
		pipeline.drawElements(cube, 8, indies, 36, mode);
		//第二个
		model = rotate(-15,-15,0) * moveto(3, 0, 0) ;
		shader->setModel(model);
		pipeline.drawElements(cube, 8, indies, 36, mode);

		//第三个
		model = rotate(-15,-15,0) * moveto(0, 3, 0) ;
		shader->setModel(model);
		pipeline.drawElements(cube, 8, indies, 36, mode);

		model = rotate(-15,-15,0) * moveto(0, -3, 0) ;
		shader->setModel(model);
		pipeline.drawElements(cube, 8, indies, 36, mode);

		*/
		//原点
		auto model = scale(5, 5, 5);
		shader->setModel(model);
		pipeline.drawElements(cube, 8, indies, 36, mode);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			//handle events
			if (event.type == SDL_QUIT) {
				quit = true;
			}else if (event.type == SDL_MOUSEMOTION) {
				int px = event.motion.x;
				int py = event.motion.y;
			}else if (event.type == SDL_KEYDOWN) {
				char key = event.key.keysym.sym;
				float moveSpeed = 0.8;
				switch (key) {
					case 'a':
						campos.x -= moveSpeed;
						break;
					case 's':
						campos.y -= moveSpeed;
						break;
					case 'w':
						campos.y += moveSpeed;
						break;
					case 'd':
						campos.x += moveSpeed;
						break;
				}
				view = lookat(campos, focus, up);
				shader->setCamera(view);
				//printf("camera:");
				//print(campos,3);
				
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
