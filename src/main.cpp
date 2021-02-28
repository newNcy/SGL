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
	auto ground = genGround(30, 30);
	
	auto loader = std::make_shared<OBJLoader>();
	auto nanosuit = loader->load("../resource/model/nanosuit/nanosuit.obj");
	auto vikingroom = loader->load("../resource/model/vikingroom/vikingroom.obj");
    SkinnedModel nano;
    nano.load("../resource/model/vikingroom/vikingroom.obj");
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

    auto animationShader = std::make_shared<AnimationShader>();
	animationShader->parallelLights["sun"] = 
	{
		{0, 0, 1},
		{1.f, 1.f, 1.f}
	};
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

    Vec3f red = {1, 0,0};
    Vec3f green = {0, 1,0};
    Vec3f blue = {0, 0,1};

    std::vector<Vertex> axis = {
        {{0,0,0},red}, {red, red},
        {{0,0,0},green}, {green, green},
        {{0,0,0},blue}, {blue, blue},
    };

    bool r = true;

	while (!quit) {
		{
			PROFILE(frame);
			{
				PROFILE(rendering);
				pipeline.clearColor(.1f, .1f, .1f);
				pipeline.clearDepth(1.f);

                //draw axis
				colorShader->setCamera(view);
                pipeline.useShader(colorShader);
                pipeline.drawArray(&axis[0], 6, DrawMode::LINE);

                //draw model
                Mat4f model;
                animationShader->setCamera(view);
                animationShader->setModel(model);
                pipeline.useShader(animationShader);
                for (auto & mesh : nano.meshes) {
					animationShader->setMaterial(mesh.material);
					pipeline.drawElements(mesh.vertices.data(), sizeof(SkinnedVertex), mesh.vertices.size(), mesh.indices.data(), mesh.indices.size(), DrawMode::TRIANGLE);	
                }
                
                model = rotate(0, int(180 + ang)%360, 0) * moveto(-10, 0,0);
                modelShader->setModel(model);
				modelShader->setCamera(view);
                pipeline.useShader(modelShader);
				for (auto & mesh : nanosuit->meshs) {
					//Profiler _(mesh->name.c_str());
					modelShader->setMaterial(mesh->material);
					pipeline.drawArray(&mesh->verties[0], mesh->verties.size(), DrawMode::TRIANGLE);	
				}
                /*
				
                Mat4f model;
				colorShader->setModel(model);
                pipeline.useShader(colorShader);


                model = rotate(0, int(180 + ang)%360, 0) * moveto(-5, 0,0);
                modelShader->setModel(model);
                pipeline.useShader(modelShader);
				for (auto & mesh : nanosuit->meshs) {
					//Profiler _(mesh->name.c_str());
					modelShader->setMaterial(mesh->material);
					pipeline.drawArray(&mesh->verties[0], mesh->verties.size(), DrawMode::TRIANGLE);	
				}
				colorShader->setModel(model);
                pipeline.useShader(colorShader);
                debug.drawBoundingBox(pipeline, nanosuit->boundingBox, colorShader); 

                model = rotate(0, 180,0) * moveto(10, 0,0);
				modelShader->setModel(model);
                pipeline.useShader(modelShader);
				for (auto & mesh : vikingroom->meshs) {
					//Profiler _(mesh->name.c_str());
					modelShader->setMaterial(mesh->material);
					pipeline.drawArray(&mesh->verties[0], mesh->verties.size(), mode);	
                }
				colorShader->setModel(model);
                debug.drawBoundingBox(pipeline, vikingroom->boundingBox, colorShader); 
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
                            case SDLK_ESCAPE:
                                quit = true;
                                break;
                            case SDLK_SPACE:
                                r = !r; 
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

            if (r) {
                ang += ms * 30 * 0.001;
            }
        }
		if (useTime >= 1000) {
			lastfps = frameCount/(useTime*0.001);
			useTime = 0;
			frameCount = 0;
		}
        char title[128] = {0};
        sprintf(title, "fps %3.1f", lastfps);
        SDL_SetWindowTitle(sgl.winPtr(), title);
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
