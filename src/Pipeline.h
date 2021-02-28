#pragma once
#include <vector>
#include <memory>
#include "FrameBuffer.h"
#include "maths.h"
#include "Shader.h"

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

V2f lerp(const V2f & start, const V2f & end, float factor);

enum class DrawMode
{
    POINT,
	LINE,
	LINE_STRIP,
	TRIANGLE,
	TRIANGLE_STRIP,
};

enum class BackFaceCullingMode
{
	DISABLE,
	CLOCKWISE,
	COUTER_CLOCKWISE
};

class SGLPipeline
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
		
		void drawPoint(const V2f & point);
		void drawTriangle0(std::vector<V2f> & points);
		void drawLine(std::vector<V2f> & points);
		void drawTriangle(const V2f & a, const V2f & b, const V2f & c);
		void drawElements(const void * verties, uint32_t stride, size_t count, unsigned int * indices, size_t indies, DrawMode drawMode);

		BackFaceCullingMode backFaceCulling = BackFaceCullingMode::CLOCKWISE;
	private:
        bool testDepth(int x, int y, float depth);
        void writeFragment(int x, int y, float depth, const Vec4f & color);
		bool backCulling(const std::vector<V2f> & points);
		void draw(const std::vector<Vertex> & verts, DrawMode mode);
		int xy2idx(int x, int y) const;
		bool needClip(const Vec3f & pos);
		Vec2i viewPortTrans(const Vec4f & pos);

		FrameBuffer * currentFrame = nullptr;
		std::shared_ptr<SGLShader> shader;
};
