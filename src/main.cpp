#include <cstdio>
#include <SDL.h>
#include <algorithm>
#include "FrameBuffer.h"
#include <math.h>
#include "SGL.h"
#include "maths.h"
#include "debug.h"
#include "geometry.h"
#include "ModelLoader.h"

int main(int argc, char * argv[])
{
	initSGL();

	SGLContext sgl;
	sgl.init("wai bi ba bo", WINPOS_CENTER, WINPOS_CENTER, 800, 800);
	SGLPineline pipeline;
	pipeline.makeFrameBuffer(800, 800);

	auto textureShader = std::make_shared<TextureShader>();
	auto colorShader = std::make_shared<NormalShader>();
	auto modelShader = std::make_shared<ModelShader>();

	auto cube = genCube();
	auto sphare = genSphare(1.f);
	auto ground = genGround(20, 20);
	
	auto loader = std::make_shared<OBJLoader>();
	auto nanosuit = loader->load("../resource/model/nanosuit/nanosuit.obj");

	int dis = (nanosuit->boundingBox.max.y - nanosuit->boundingBox.min.y)/2;
	//第一个
	Vec3f campos(0, dis, -dis*2.5);
	Vec3f up(0, 1, 0);
	Vec3f lookDir(0, 0, 1);
	float yaw = 0.f, pitch = 0.f;
	auto view = lookat(campos, campos + lookDir, up);


	bool quit = false;
	auto mode = DrawMode::SGL_TRIANGLE;

	if (SDL_SetRelativeMouseMode((SDL_bool)true)) {
		return 0;
	}

	long frameCount = 0;
	long useTime = 0;
	float fps = 0.f;

	auto lastTime = std::chrono::system_clock::now();
	float ang = 0;
	float lastfps = 0;

	pipeline.useShader(modelShader);
	//增加阳光
	modelShader->parallelLights["light"] = 
	{
		{0, 0, 1},
		{1.f, 1.f, 1.f}
	};
	
	while (!quit) {
		{
			PROFILE(frame);
			{
				PROFILE(rendering);
				pipeline.clearColor(.5f, .5f, .5f);
				pipeline.clearDepth(1.f);

				auto model = rotate(0, 150, 0);
				modelShader->setModel(model);
				modelShader->setCamera(view);

				for (auto & mesh : nanosuit->meshs) {
					modelShader->setMaterial(mesh->material);
					pipeline.drawArray(&mesh->verties[0], mesh->verties.size(), mode);	
				}
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
						float sense = 0.05f;
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

					}
					view = lookat(campos, campos + lookDir, up);
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
		for (auto & mesh : nanosuit->meshs) {
			printf("%s ", mesh->name.c_str());
			if (mesh->material) {
				printf("usemtl %s ", mesh->material->name.c_str());
			}
			printf("\n");
		}
		fflush(stdout);
	}

	sgl.destroy();
	releaseSGL();
	return 0;
}
