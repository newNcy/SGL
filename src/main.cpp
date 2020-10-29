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
		{{-1.f, 1.f, -1.f}, {1.f,0.f,0.f},{0,1}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f, -1.f}, {1.f,0.f,0.f},{1,1}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f, -1.f}, {1.f,0.f,0.f},{1,0}, {0.f,0.f,0.f}},
		{{-1.f,-1.f, -1.f}, {1.f,0.f,0.f},{0,0}, {0.f,0.f,0.f}},

		{{-1.f, 1.f,  1.f}, {0.f,1.f,0.f},{0,1}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f,  1.f}, {0.f,1.f,0.f},{1,1}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {0.f,1.f,0.f},{1,0}, {0.f,0.f,0.f}},
		{{-1.f,-1.f,  1.f}, {0.f,1.f,0.f},{0,0}, {0.f,0.f,0.f}}
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
				projection = perspective(90, 1.f, 3.f, 100);
			}

			Vec4f onVertex(const Vec3f & pos) override
			{
				Vec4f ret = Vec4f(pos,1) * model * view * projection;
				return ret;
			}
	};
	class TextureShader : public NormalShader
	{
		protected:
			std::shared_ptr<Texture> texture;
			Vec4f onFragment(const V2f & v) override
			{
				return texture->sample(v.uv.u, v.uv.v);
			}
	};
	auto shader = std::make_shared<NormalShader>();
	pipeline.useShader(shader);
	
	//第一个
	Vec3f campos(0, 0, -6);
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
	while (!quit) {
		PROFILE(frame);
		{
			PROFILE(rendering);
			pipeline.clearColor(.5f, .5f, .5f);
			pipeline.clearDepth(1.f);

			auto model = scale(5,5,5);
			shader->setModel(model);
			pipeline.drawElements2(cube, 8, indies, 36, mode);
			
			model = rotate(-15,-15,0) * moveto(3, 0, 0) ;
			shader->setModel(model);
			//pipeline.drawElements2(cube, 8, indies, 36, mode);
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
					if (pitch > 89) {
						pitch = 89;
					}
					if (pitch < -89) {
						pitch = -89;
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
				shader->setCamera(view);
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
