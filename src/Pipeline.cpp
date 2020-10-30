#include "Pipeline.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#include "debug.h"

void SGLPineline::makeFrameBuffer(int width, int height)
{
	//尺寸一样就重用
	if (currentFrame ) {
		if (currentFrame->getWidth() == width && currentFrame->getHeight() == height) {
			return;
		}else {
			delete currentFrame;
		}
	}
	currentFrame = new FrameBuffer(width, height);
}

const FrameBuffer & SGLPineline::getCurrentFrameBuffer() const
{
	assert(currentFrame);
	return *currentFrame;
}
		
void SGLPineline::clearColor(float r, float g, float b)
{
	if (!currentFrame) {
		return;
	}

	for (int y = 0; y < currentFrame->getHeight(); ++ y) {
		for (int x = 0; x < currentFrame->getWidth(); ++ x) {
			currentFrame->setPixel(x, y, r, g, b);
		}
	}
}

void SGLPineline::clearDepth(float d)
{
	if (!currentFrame) {
		return;
	}
	for (int y = 0; y < currentFrame->getHeight(); ++ y) {
		for (int x = 0; x < currentFrame->getWidth(); ++ x) {
			currentFrame->setDepth(x, y, d);
		}
	}
}


void SGLPineline::drawScreenLine(const ScreenPoint & a, const ScreenPoint & b, bool testz)
{
	if (!currentFrame) {
		return;
	}

	auto s = a.position;
	auto e = b.position;
	auto ca = a.color;
	auto cb = b.color;
	auto da = a.depth;
	auto db = b.depth;

	int dy = e.y - s.y;
	int dx = e.x - s.x;
	bool steep = abs(dy) > abs(dx);
	bool needEx = false;
	if (steep) {
		std::swap(dx, dy);
		std::swap(s.x, s.y);
		std::swap(e.x, e.y);
	}

	if (s.x > e.x) {
		std::swap(s.x, e.x);
		std::swap(s.y, e.y);
	}

	float k = abs(float(dy) / dx);
	float err = 0;
	int ystep = e.y > s.y? 1 : -1;

	while (s.x != e.x) {
		auto left = float(e.x - s.x)/dx;
		Vec3f color = lerp(ca, cb, (1.f-left));
		float depth = lerp(da, db, (1.f-left));
		int x = s.x, y = s.y;
		if (steep) {
			std::swap(x, y);
		}
		if (!testz || currentFrame->getDepth(x, y) > depth) {
			currentFrame->setPixel(x, y, color);
			currentFrame->setDepth(x, y, depth);
		}
		err += k;
		s.x ++;
		if (err > 0.5) {
			s.y += ystep;
			err -= 1;
		}
	}
}


BoundingBox bbox(const FrameBuffer & frame, const Vec2i & a, const Vec2i & b,const Vec2i & c)
{
	BoundingBox ret = 
	{
		{frame.getWidth()-1, frame.getHeight()-1},
		{0, 0}
	};
	std::vector<Vec2i> ps = {a, b, c};
	for (auto & p : ps) {
		for (int i = 0; i < 2; ++ i) {
			if (p[i] < ret.min[i]) {
				ret.min[i] = p[i];
			}
			if (p[i] > ret.max[i]) {
				ret.max[i] = p[i];
			}
		}
	}
	return ret;
}

V2f lerp(const V2f & start, const V2f & end, float factor)
{
	V2f ret;
	ret.position = lerp(start.position, end.position, factor);
	ret.color = lerp(start.color, end.color, factor);
	ret.uv = lerp(start.uv, end.uv, factor);
	ret.norm = lerp(start.norm, end.norm, factor);
	return ret;
}

float calcS(const Vec2i & a, const Vec2i & b,const Vec2i & c)
{
	auto ab = b-a;
	auto ac = c-a;
	float res = fabs(ab.x*ac.y-ab.y*ac.x);
	return res*0.5f;
}

void SGLPineline::drawScreenTriangle(const ScreenPoint & a, const ScreenPoint & b, const ScreenPoint & c, bool testz)
{
	if (!currentFrame) {
		return;
	}

	auto A = a.position;
	auto B = b.position;
	auto C = c.position;

	auto bb = bbox(*currentFrame, A, B, C);
	float S = calcS(A, B, C);
	if (!S) {
		return;
	}

	for (int x = bb.min.x; x <= bb.max.x; ++ x) {
		for (int y = bb.min.y; y <= bb.max.y; ++ y) {
			Vec2i p(x, y);
			float s1 = calcS(B,p,C);
			float s2 = calcS(A,p,C);
			float s3 = calcS(A,p,B);
			
			float w1= s1/S;
			float w2 = s2/S;
			float w3 = s3/S;

			float depth = a.depth*w1+ b.depth*w2 + c.depth*w3;
			auto color = a.color*w1+ b.color*w2+ c.color*w3;
			if (s1+s2+s3 <= S) {
				float depthBuf = currentFrame->getDepth(x, y);
				if (!testz || depthBuf > depth) {
					currentFrame->setPixel(x, y, color);
					currentFrame->setDepth(x, y, depth);
				}
			}		
		}
	}
}
	

bool SGLPineline::needClip(const Vec3f & pos)
{
	for (int i = 0; i < 3; ++ i) {
		if (pos[i] < -1 || pos[i] > 1) {
			return true;
		}
	}
	return false;
}

Vec2i SGLPineline::viewPortTrans(const Vec4f & pos)
{
	float x = (pos.x + 1)*0.5f;
	float y = (-pos.y + 1)*0.5f;

	return Vec2i(x*(currentFrame->getWidth()-1), y*(currentFrame->getHeight()-1));
}
		
void SGLPineline::drawArrayLine(const Vertex * verties, size_t count, DrawMode drawMode)
{
}


void SGLPineline::draw(const std::vector<Vertex> & verts, DrawMode mode)
{
	//图元装配
	for (int i = 0; i < verts.size()/3; ++ i) {
		std::vector<ScreenPoint> screenPoints;
		for (int j = 0; j < 3; ++ j) {
			auto & vert = verts[i*3 + j];
			if (needClip(vert.position)) {
				continue;
			}
			Vec4f color;// = shader->onFragment(vert);
			ScreenPoint p;
			p.position = viewPortTrans(vert.position);
			p.color = Vec3f(color.r, color.g, color.b);
			p.depth  = vert.position.z;
			screenPoints.push_back(p);
		}
		if (screenPoints.size() == 3) {
			if (mode == DrawMode::SGL_TRIANGLE) {
				drawScreenTriangle(screenPoints[0], screenPoints[1], screenPoints[2], true);
			} else if (mode == DrawMode::SGL_LINE) {
				drawScreenLine(screenPoints[0], screenPoints[1]);
				drawScreenLine(screenPoints[1], screenPoints[2]);
				drawScreenLine(screenPoints[2], screenPoints[0]);
			}

		}
	}
}

void SGLPineline::drawTriangle(const V2f & a, const V2f & b, const V2f & c)
{
	if (!currentFrame) {
		return;
	}

	auto clipA = a.position/a.position.w;
	auto clipB = b.position/b.position.w;
	auto clipC = c.position/c.position.w;

	auto A = viewPortTrans(clipA);
	auto B = viewPortTrans(clipB);
	auto C = viewPortTrans(clipC);

	auto bb = bbox(*currentFrame, A, B, C);
	float S = calcS(A, B, C);
	if (!S) {
		return;
	}

	for (int x = bb.min.x; x <= bb.max.x; ++ x) {
		for (int y = bb.min.y; y <= bb.max.y; ++ y) {
			Vec2i p(x, y);
			float s1 = calcS(B,p,C);
			float s2 = calcS(A,p,C);
			float s3 = calcS(A,p,B);
			
			float w1= s1/S;
			float w2 = s2/S;
			float w3 = s3/S;

			float depth = clipA.z*w1+ clipB.z*w2 + clipC.z*w3;
			V2f f;
			f.position = a.position*w1 + b.position*w2 + c.position*w3;
			f.uv = a.uv/a.position.w*w1 + b.uv/b.position.w*w2 + c.uv/c.position.w*w3;
			f.uv = f.uv*f.position.w;
			f.norm = a.norm*w1 + b.norm*w2 + c.norm*w3;
			f.color = a.color*w1+ b.color*w2+ c.color*w3;

			Vec4f color;
			shader->onFragment(f, color);
			
			if (s1+s2+s3 <= S) {
				float depthBuf = currentFrame->getDepth(x, y);
				if (depthBuf > depth) {
					currentFrame->setPixel(x, y, color);
					currentFrame->setDepth(x, y, depth);
				}
			}		
		}
	}
}

std::vector<V2f> SutherlandHodgeman(std::vector<V2f> & out, int count)
{
	if (out.size() < 2) {
		return out;
	}

	const std::vector<Vec4f> planes = 
	{
		{1, 0, 0, 1},
		{-1, 0, 0, 1},
		{0, 1, 0, 1},
		{0, -1, 0, 1},
		{0, 0, 1, 1},
		{0, 0, -1, 1}
	};

	std::vector<V2f> in;
	int idx = 0;
	for (auto & plane : planes) {
		std::swap(in, out);
		out.clear();
		for (int i = 0, j = 1; i < in.size(); ++ i, ++ j) {
			if (j == in.size()) {
				j = 0;
			}
			float di = in[i].position * plane;
			float dj = in[j].position * plane;
			int code = (dj >= 0.001) | ((di>= 0.001)<<1);
			switch(code) {
				case 0: //都不在
					break;
				case 1: //i不在 j在
					{
						V2f clip = lerp(in[i], in[j], di/(di-dj));
						out.push_back(clip);
						out.push_back(in[j]);
					}
					break;
				case 2: //i在j不在
					{
						V2f clip = lerp(in[i], in[j], di/(di-dj));
						out.push_back(clip);
					}
					break;
				case 3:
					{
						out.push_back(in[j]);
					}
					break;
			}
		}
	}
	return out;
}

void SGLPineline::drawArray(const Vertex * verties, size_t count, DrawMode drawMode)
{
	if (!verties || !currentFrame || !shader) {
		return;
	}

	std::vector<V2f> points;
	for (int i = 0; i < count; ++ i) {
		Vertex v = verties[i];
		V2f out;
		out.position = v.position;
		out.color = v.color;
		out.uv = v.uv;
		out.norm = v.norm;
		shader->onVertex(v, out);
		points.push_back(out);

		if (points.size() == 3) {
			PROFILE(triangle);
			std::vector<V2f> clips = SutherlandHodgeman(points, 3);
			if (clips.size()) {

				int last = 1;
				while (last < clips.size() - 1) {
					drawTriangle(clips[0], clips[last], clips[last+1]);
					last ++;
				}
			}
			points.clear();
		}
	}
}
void SGLPineline::drawElements2(const Vertex * verties, size_t count, unsigned int * indices, size_t indiesCount, DrawMode drawMode)
{
	if (!verties || !indices || !currentFrame || !shader) return;

	std::vector<V2f> points;
	for (int i = 0; i < indiesCount; ++ i) {
		Vertex v = verties[indices[i]];
		V2f out;
		out.position = v.position;
		out.color = v.color;
		out.uv = v.uv;
		out.norm = v.norm;
		shader->onVertex(v, out);
		points.push_back(out);

		if (points.size() == 3) {
			std::vector<V2f> clips = SutherlandHodgeman(points, 3);
			if (clips.size()) {

				int last = 1;
				while (last < clips.size() - 1) {
					drawTriangle(clips[0], clips[last], clips[last+1]);
					last ++;
				}
			}
			points.clear();
		}
	}

}

Texture::Texture(const char * path)
{
	data = stbi_load(path, &width, &height, &n, 0);
}

Vec4f Texture::sample(float u, float v)
{
	int x = u*(width-1);
	int y = (1-v)*(height-1);
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= width) x = width -1;
	if (y >= height) y = height-1;
	unsigned char * p = data + y*n*width + x*n;
	Vec4f ret(p[0]/255.f, p[1]/255.f,p[2]/255.f, 1.f);
	return ret;
}

Texture::~Texture()
{
	stbi_image_free(data);
}
