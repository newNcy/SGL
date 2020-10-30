#pragma once 
#include "maths.h"
#include <memory>

struct Vertex
{
	Vec3f position;	//位置
	Vec3f color;	//颜色
	Vec2f uv;		//纹理坐标
	Vec3f norm;		//法向量
};

struct V2f 
{
	Vec4f position;
	Vec3f color;	//颜色
	Vec2f uv;		//纹理坐标
	Vec3f norm;		//法向量
};

struct Texture
{
	Texture(const char * path);
	Vec4f sample(float u, float v);
	~Texture();

	int width = 0;
	int height = 0;
	int n = 0;
	unsigned char * data = nullptr;
};

class SGLShader
{	
	public:
		virtual void onVertex(const Vertex & vert, V2f & out) 
		{ 
		}
		virtual void onFragment(const V2f & v2f, Vec4f & color) 
		{ color = Vec4f(v2f.color,1); }
		~SGLShader() {}
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

		void onFragment(const V2f & v, Vec4f & color) override
		{
			color = texture->sample(v.uv.u, v.uv.v);
		}
};

