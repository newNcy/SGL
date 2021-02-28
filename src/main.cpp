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
    int width = 800, height = 800;
	sgl.init("wai bi ba bo", WINPOS_CENTER, WINPOS_CENTER, width, height);
	SGLPipeline pipeline;
	pipeline.makeFrameBuffer(width, height);

	auto textureShader = std::make_shared<TextureShader>();
	auto colorShader = std::make_shared<NormalShader>();
	auto modelShader = std::make_shared<ModelShader>();

	auto cube = genCube();
	auto sphare = genSphare(1.f);
	auto ground = genGround(20, 20);
	
	auto loader = std::make_shared<OBJLoader>();
	auto nanosuit = loader->load("../resource/model/nanosuit/nanosuit.obj");
	//auto nanosuit = loader->load("../resource/model/file.obj");

	int dis = (nanosuit->boundingBox.max.y - nanosuit->boundingBox.min.y)/2;
	//第一个
	Vec3f campos(0, dis, -dis*2.5);
	Vec3f up(0, 1, 0);
	Vec3f lookDir(0, 0, 1);
	float yaw = 0.f, pitch = 0.f;
	auto view = lookat(campos, campos + lookDir, up);


	bool quit = false;
	auto mode = DrawMode::TRIANGLE;


	long frameCount = 0;
	long useTime = 0;
	float fps = 0.f;

	auto lastTime = std::chrono::system_clock::now();
	float ang = 0;
	float lastfps = 0;

	pipeline.useShader(modelShader);
	//增加阳光
	modelShader->parallelLights["sun"] = 
	{
		{0, 0, 1},
		{1.f, 1.f, 1.f}
	};
	
    /*
	modelShader->parallelLights["moon"] = 
	{
		{0, 1, -1},
		{1.f, 0.f, 1.f}
	};
    */

    DebugRenderer debug;
	while (!quit) {
		{
			PROFILE(frame);
			{
				PROFILE(rendering);
				pipeline.clearColor(.0f, .0f, .0f);
				pipeline.clearDepth(1.f);

				auto model = rotate(0, 180, 0) * moveto(0,1, 0);
				colorShader->setCamera(view);
				colorShader->setModel(model);
                pipeline.useShader(colorShader);
                //pipeline.drawArray(&ground[0], 6, DrawMode::TRIANGLE);
				//ang += 4;
                pipeline.useShader(modelShader);
				modelShader->setModel(model);
				modelShader->setCamera(view);

				for (auto & mesh : nanosuit->meshs) {
					Profiler _(mesh->name.c_str());
					modelShader->setMaterial(mesh->material);
					pipeline.drawArray(&mesh->verties[0], mesh->verties.size(), mode);	
				}
                debug.drawBoundingBox(pipeline, nanosuit->boundingBox, colorShader); 
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
		//printf("%3.1f fps\n", lastfps);
		for (auto & mesh : nanosuit->meshs) {
			//printf("%s ", mesh->name.c_str());
			if (mesh->material) {
				//printf("usemtl %s ", mesh->material->name.c_str());
			}
			//printf("\n");
		}
		//printf("yaw %.2f pitch %.2f, look dir (%.2f %.2f %.2f)\n", yaw, pitch, lookDir.x, lookDir.y, lookDir.z);
		fflush(stdout);
	}

	sgl.destroy();
	releaseSGL();
	return 0;
}
