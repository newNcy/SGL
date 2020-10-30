#pragma once
#include <vector>
#include <memory>
#include "FrameBuffer.h"
#include "maths.h"

struct Vertex
{
	Vec3f position;	//位置
	Vec3f color;	//颜色
	Vec2f uv;		//纹理坐标
	Vec3f norm;		//法向量
};

struct BoundingBox
{
	Vec2i min;
	Vec2i max;
};

struct ScreenPoint
{
	Vec2i position;
	Vec3f color;
	float depth = 0;
};

struct V2f 
{
	Vec4f position;
	Vec3f color;	//颜色
	Vec2f uv;		//纹理坐标
	Vec3f norm;		//法向量
};

V2f lerp(const V2f & start, const V2f & end, float factor);

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

enum class DrawMode
{
	SGL_LINE,
	SGL_TRIANGLE,
};


class SGLPineline
{
	public:
		void makeFrameBuffer(int width, int height);
		void useShader(std::shared_ptr<SGLShader> shader) { this->shader = shader; }
		const FrameBuffer & getCurrentFrameBuffer() const;
		std::shared_ptr<SGLShader> getCurrentShader() { return shader; }

		void drawScreenLine(const ScreenPoint & a, const ScreenPoint & b, bool testz = false);
		void drawScreenTriangle(const ScreenPoint & a, const ScreenPoint & b, const ScreenPoint & c, bool testz = false);

		void drawArrayLine(const Vertex * verties, size_t count, DrawMode drawMode);

		void clearColor(float r, float g, float b);
		void clearDepth(float d);

		void drawArray(const Vertex * verties, size_t count, DrawMode drawMode);
		
		void drawTriangle(const V2f & a, const V2f & b, const V2f & c);
		void drawElements2(const Vertex * verties, size_t count, unsigned int * indices, size_t indies, DrawMode drawMode);

	private:
		void draw(const std::vector<Vertex> & verts, DrawMode mode);
		int xy2idx(int x, int y) const;
		bool needClip(const Vec3f & pos);
		Vec2i viewPortTrans(const Vec4f & pos);

		FrameBuffer * currentFrame = nullptr;
		std::shared_ptr<SGLShader> shader;
};