#include "Pipeline.h"
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

	PROFILE(triangle);
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
			
			float k = w1/a.position.w + w2/b.position.w + w3/c.position.w;
			w1 = w1/a.position.w/k;
			w2 = w2/b.position.w/k;
			w3 = w3/c.position.w/k;
			f.uv = a.uv*w1 + b.uv*w2 + c.uv*w3;
			f.norm = a.norm*w1 + b.norm*w2 + c.norm*w3;
			f.color = a.color*w1+ b.color*w2+ c.color*w3;
			f.worldPosition = a.worldPosition*w1+ b.worldPosition*w2+ c.worldPosition*w3;

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


bool inside(const V2f & v)
{
	auto & p = v.position;
	return p.x > -p.w && p.x < p.w && p.y > -p.w && p.y < p.w && p.z > -p.w && p.z < p.w;
}

//齐次裁剪
std::vector<V2f> SutherlandHodgeman(std::vector<V2f> & out, int count)
{
	PROFILE(SutherlandHodgeman)
	if (out.size() < 2) {
		return std::move(out);
	}

	bool allInside = true;
	for (int i = 0; i < count; ++ i) {
		allInside &= inside(out[i]);
	}

	if (allInside) {
		return std::move(out);
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
		if (!out.size()) { //剔光了
			break;
		}
		std::swap(in, out);
		out.clear();
		float pred = dot(in[0].position, plane);
		int pre = 0;
		for (int i = 1; ; ++ i) {
			if (i == in.size()) {
				i = 0;
			}
			float di = dot(in[i].position, plane);
			int code = (di >= 0.001) | ((pred >= 0.001)<<1);
			switch(code) {
				case 0: //都不在
					break;
				case 1: //i不在 j在
					{
						V2f clip = lerp(in[pre], in[i], pred/(pred-di));
						out.push_back(clip);
						out.push_back(in[i]);
					}
					break;
				case 2: //i在j不在
					{
						V2f clip = lerp(in[pre], in[i], pred/(pred-di));
						out.push_back(clip);
					}
					break;
				case 3:
					{
						out.push_back(in[i]);
					}
					break;
			}
			if (!i) {
				break;
			}
			pre = i;
			pred = di;
		}
	}
	return out;
}

bool SGLPineline::backCulling(const std::vector<V2f> & points)
{
	if (backFaceCulling == BackFaceCullingMode::DISABLE) {
		return false;
	}
	Vec3f norm;
	Vec3f a(points[0].position.x,points[0].position.y, points[0].position.z);
	Vec3f b(points[1].position.x,points[1].position.y, points[1].position.z);
	Vec3f c(points[2].position.x,points[2].position.y, points[2].position.z);

	norm = cross(c-a, b-a);
	if (backFaceCulling != BackFaceCullingMode::CLOCKWISE) {
		norm = norm * -1;
	}

	return dot(a, norm) < 0;
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
			//背面剔除下，不然都裁剪太离谱了，卡得一笔 (2070s下 6fps->12fps)
			if (!backCulling(points)) {
				std::vector<V2f> clips = SutherlandHodgeman(points, 3);
				if (clips.size()) {
					int last = 1;
					while (last < clips.size() - 1) {
						drawTriangle(clips[0], clips[last], clips[last+1]);
						last ++;
					}
				}
			}
			points.clear();
		}
	}
}

void SGLPineline::drawElements(const Vertex * verties, size_t count, unsigned int * indices, size_t indiesCount, DrawMode drawMode)
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

