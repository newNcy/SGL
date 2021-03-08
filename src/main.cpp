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

void travelNode(std::shared_ptr<SkeletonNode> root, std::vector<Vertex> & vs, const Vertex & parent = Vertex(), int tab = 0)
{
    for (int i = 0 ; i < tab; ++ i) {
        printf("  ");
    }
    Vec4f pos = {0, 0, 0, 1};
    pos = pos * root->transform;
    Vertex v = {{pos.x, pos.y, pos.z}, {1, 1, 1}};
    if (root->parent) {
        vs.push_back(parent);
    }else {
        vs.push_back(v);
    }
    vs.push_back(v);
    printf("%s (%.2f %.2f %.2f)\n", root->name.c_str(), pos.x, pos.y, pos.z);
    for (auto child : root->childs) {
        travelNode(child, vs,v, tab+1);
    }
}


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
    SkinnedModel robot;
    robot.load("../resource/model/robot/robot.fbx");
    
    AnimationSet anims;
    anims.load("../resource/model/robot/jump.fbx");

	//auto nanosuit = loader->load("../resource/model/file.obj");

	int dis = (robot.boundingBox.max.y - robot.boundingBox.min.y)/2;
	//第一个
	Vec3f campos(0, dis, dis*4);
	Vec3f up(0, 1, 0);
	Vec3f lookDir(0, 0, -1);
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
		{0, 0, -1},
		{1.f, 1.f, 1.f}
	};
	//增加阳光
	modelShader->parallelLights["sun"] = 
	{
		{0, 0, -1},
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

    for (auto & anim : anims.animations) {
        printf("%s %.2lf %.2lf\n", anim.first.c_str(), anim.second.duration, anim.second.ticksPerSecond);
    }
    auto & anim = anims.animations["mixamo.com"];
    float time = anim.duration;
    auto start = std::chrono::system_clock::now();

	while (!quit) {
		{
			PROFILE(frame);
			{
				PROFILE(rendering);
				pipeline.clearColor(.1f, .1f, .1f);
				pipeline.clearDepth(1.f);

                //draw axis
				colorShader->setCamera(view);
				colorShader->setModel(Mat4f());
                pipeline.useShader(colorShader);
                pipeline.drawArray(&axis[0], 6, DrawMode::LINE);

                Mat4f model;
                animationShader->setCamera(view);
                Quat quat(Vec3f(0, 1, 0), halfRadians(ang));
                //animationShader->setModel(quat);

                auto current = std::chrono::system_clock::now() - start;
                double sec = current.count()/1000000000.0;
                //
                //printf("%lf\n", sec);
                std::vector<Vertex> vs;
                auto frame = anim.getFrame(0, anims.skeleton);
                travelNode(anims.skeleton.root, vs);
                pipeline.drawArray(vs.data(), vs.size(), DrawMode::LINE);

                //debug.drawSkeleton(pipeline, robot.skeleton, colorShader); 
                for (auto & mesh : robot.meshes) {
					animationShader->setMaterial(mesh.material);
                    pipeline.useShader(animationShader);
					pipeline.drawElements(mesh.vertices.data(), sizeof(SkinnedVertex), mesh.vertices.size(), mesh.indices.data(), mesh.indices.size(), DrawMode::TRIANGLE);
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
						yaw += -px*sense;
						pitch += -py*sense;
						if (pitch > 89.99999) {
							pitch = 89.99999;
						}
						if (pitch < -89.99999) {
							pitch = -89.99999;
						}

						lookDir.x = cos(radians(pitch)) * -sin(radians(yaw));
						lookDir.y = sin(radians(pitch));
						lookDir.z = cos(radians(pitch)) * -cos(radians(yaw));

						lookDir = normalize(lookDir);
						//fflush(stdout);
					}else if (event.type == SDL_KEYDOWN) {
						char key = event.key.keysym.sym;
						float moveSpeed = 1;
						switch (key) {
							case 'a':
								campos = campos + normalize(cross(up, lookDir))*moveSpeed;
								break;
							case 's':
								campos = campos - lookDir * moveSpeed;
								break;
							case 'w':
								campos = campos + lookDir * moveSpeed;
								break;
							case 'd':
								campos = campos + normalize(cross(lookDir, up));
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
