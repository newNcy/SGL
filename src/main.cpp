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

	auto cube = genCube();
	class NormalShader : public SGLShader
	{
		const float pi = 3.1415926;
		protected:
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

			void onVertex(const Vertex & vert, V2f & out) override
			{
				out.position = Vec4f(vert.position,1) * model * view * projection;
			}
	};
	class TextureShader : public NormalShader
	{
		protected:
			std::shared_ptr<Texture> texture;
		public:
			TextureShader()
			{
				texture.reset(new Texture("../resource/image/container.jpg"));
			}

			void onVertex(const Vertex & vert, V2f & out) override
			{
				out.position = Vec4f(vert.position,1) * model * view * projection;
			}

			void onFragment(const V2f & v, Vec4f & color) override
			{
				color = texture->sample(v.uv.u, v.uv.v);
			}
	};
	auto shader = std::make_shared<TextureShader>();
	auto colorShader = std::make_shared<NormalShader>();

	pipeline.useShader(shader);

	auto sphare = genSphare(1.f);
	//第一个
	Vec3f campos(0, 0, -30);
	Vec3f up(0, 1, 0);
	Vec3f lookDir(0, 0, 1);
	float yaw = 0.f, pitch = 0.f;
	auto view = lookat(campos, campos + lookDir, up);

	shader->setCamera(view);

	bool quit = false;
	auto mode = DrawMode::SGL_TRIANGLE;

	float xr = 0, yr = 0, zr = 0;

	if (SDL_SetRelativeMouseMode((SDL_bool)true)) {
		return 0;
	}

	long frameCount = 0;
	long useTime = 0;
	float fps = 0.f;

	pipeline.clearColor(.5f, .5f, .5f);
	pipeline.clearDepth(1.f);
	auto lastTime = std::chrono::system_clock::now();
	sgl.swapBuffer(pipeline.getCurrentFrameBuffer());
	float ang = 0;
	while (!quit) {
		PROFILE(frame);
		{
			PROFILE(rendering);
			pipeline.clearColor(.5f, .5f, .5f);
			pipeline.clearDepth(1.f);
			ang += 5;
			auto model = scale(5,5,5) * rotate(ang, ang, ang);
			shader->setModel(model);
			shader->setCamera(view);
			pipeline.useShader(shader);
			pipeline.drawArray(&cube[0], 36, mode);	


			model = scale(5,5,5) * moveto(20, 0, 0);
			colorShader->setModel(model);
			colorShader->setCamera(view);
			pipeline.useShader(colorShader);
			pipeline.drawArray(&sphare[0], sphare.size(), mode);
			/*
			auto model = rotate(15,15,0);
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
		}

		/*
		model = scale(5, 5, 5) ;
		shader->setModel(model);
		pipeline.drawElements(cube, 8, indies, 36, mode);
		*/

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
		if (useTime >= 1000) {
			printf("%3.1f fps\n", frameCount/(useTime*0.001));
			useTime = 0;
			frameCount = 0;
		}
	}

	sgl.destroy();
	releaseSGL();
	return 0;
}
