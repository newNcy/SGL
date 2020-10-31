#include <cstdio>
#include <SDL.h>
#include <algorithm>
#include "FrameBuffer.h"
#include <math.h>
#include "SGL.h"
#include "maths.h"
#include "debug.h"
#include "geometry.h"

int main(int argc, char * argv[])
{
	initSGL();

	SGLContext sgl;
	sgl.init("wai bi ba bo", WINPOS_CENTER, WINPOS_CENTER, 800, 800);
	SGLPineline pipeline;
	pipeline.makeFrameBuffer(800, 800);

	auto textureShader = std::make_shared<TextureShader>();
	auto colorShader = std::make_shared<NormalShader>();

	auto cube = genCube();
	auto sphare = genSphare(1.f);
	auto ground = genGround(20, 20);

	//第一个
	Vec3f campos(10, 5, 0);
	Vec3f up(0, 1, 0);
	Vec3f lookDir(0, 0, 1);
	float yaw = 0.f, pitch = 0.f;
	auto view = lookat(campos, campos + lookDir, up);


	bool quit = false;
	auto mode = DrawMode::SGL_TRIANGLE;

	float xr = 0, yr = 0, zr = 0;

	if (SDL_SetRelativeMouseMode((SDL_bool)true)) {
		return 0;
	}

	long frameCount = 0;
	long useTime = 0;
	float fps = 0.f;

	auto lastTime = std::chrono::system_clock::now();
	float ang = 0;
	float lastfps = 0;
	while (!quit) {
		{
			PROFILE(frame);
			{
				PROFILE(rendering);
				pipeline.clearColor(.5f, .5f, .5f);
				pipeline.clearDepth(1.f);
				ang += 5;

				pipeline.useShader(textureShader);

				Mat4f model;
				textureShader->setCamera(view);

				//绘制盒子
				model = moveto(5, 2, 5);
				textureShader->setModel(model);
				pipeline.drawArray(&cube[0], 36, mode);	

				//绘制地板
				pipeline.useShader(colorShader);
				colorShader->setCamera(view);

				model = Mat4f();
				colorShader->setModel(model);
				pipeline.drawArray(&ground[0], 6, mode);


				/*
				//绘制球体
				model = moveto(10,1, 5);
				colorShader->setModel(model);
				pipeline.drawArray(&sphare[0], sphare.size(), mode);
				*/
			}

			SDL_Event event;

			{
				PROFILE(event_handle);
				while (SDL_PollEvent(&event)) {
					//handle events
					if (event.type == SDL_QUIT) {
						quit = true;
					}else if (event.type == SDL_MOUSEMOTION) {
						float px = event.motion.xrel;
						float py = event.motion.yrel;
						//printf("x:%f y:%f\n", px, py);
						float sense = 0.1f;
						yaw += px*sense;
						pitch += -py*sense;
						if (pitch > 89.99999) {
							pitch = 89.99999;
						}
						if (pitch < -89.99999) {
							pitch = -89.99999;
						}

						lookDir.x = cos(radians(pitch)) * sin(radians(yaw));
						lookDir.y = sin(radians(pitch));
						lookDir.z = cos(radians(pitch)) * cos(radians(yaw));

						lookDir = normalize(lookDir);
						//fflush(stdout);
					}else if (event.type == SDL_KEYDOWN) {
						char key = event.key.keysym.sym;
						float moveSpeed = 1;
						switch (key) {
							case 'a':
								campos = campos + normalize(cross(lookDir, up))*moveSpeed;
								break;
							case 's':
								campos = campos - lookDir * moveSpeed;
								break;
							case 'w':
								campos = campos + lookDir * moveSpeed;
								break;
							case 'd':
								campos = campos + normalize(cross(up, lookDir));
								break;
						}
						//printf("camera:");
						//print(campos,3);

					}
					view = lookat(campos, campos + lookDir, up);
					//do things			
					//rendering
				}
			}

			sgl.swapBuffer(pipeline.getCurrentFrameBuffer());
			frameCount ++;
			auto current = std::chrono::system_clock::now();
			auto use = current - lastTime;
			lastTime = current;
			long ms = use.count() / 1000000;
			useTime += ms;
		}
		if (useTime >= 1000) {
			lastfps = frameCount/(useTime*0.001);
			useTime = 0;
			frameCount = 0;
		}
		printf("%3.1f fps\n", lastfps);
		fflush(stdout);
	}

	sgl.destroy();
	releaseSGL();
	return 0;
}
