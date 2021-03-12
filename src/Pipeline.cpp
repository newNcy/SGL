#include "Pipeline.h"
#include "debug.h"

void SGLPipeline::makeFrameBuffer(int width, int height)
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

const FrameBuffer & SGLPipeline::getCurrentFrameBuffer() const
{
	assert(currentFrame);
	return *currentFrame;
}
		
void SGLPipeline::clearColor(float r, float g, float b)
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

void SGLPipeline::clearDepth(float d)
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


void SGLPipeline::drawScreenLine(const ScreenPoint & a, const ScreenPoint & b, bool testz)
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

void SGLPipeline::drawScreenTriangle(const ScreenPoint & a, const ScreenPoint & b, const ScreenPoint & c, bool testz)
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
	

bool SGLPipeline::needClip(const Vec3f & pos)
{
	for (int i = 0; i < 3; ++ i) {
		if (pos[i] < -1 || pos[i] > 1) {
			return true;
		}
	}
	return false;
}

Vec2i SGLPipeline::viewPortTrans(const Vec4f & pos)
{
	float x = (pos.x + 1)*0.5f;
	float y = (-pos.y + 1)*0.5f;

	return Vec2i(x*(currentFrame->getWidth()-1), y*(currentFrame->getHeight()-1));
}
		

bool inside(const V2f & v)
{
    auto & p = v.position;
    return p.x > -p.w && p.x < p.w && p.y > -p.w && p.y < p.w && p.z > -p.w && p.z < p.w;
}

//齐次裁剪
std::vector<V2f> SutherlandHodgeman(std::vector<V2f> & out, int count)
{
    PROFILE(SutherlandHodgeman);
    std::vector<V2f> in;

    const std::vector<Vec4f> planes = 
    {
        {1, 0, 0, 1},
        {-1, 0, 0, 1},
        {0, 1, 0, 1},
        {0, -1, 0, 1},
        {0, 0, 1, 1},
        {0, 0, -1, 1}
    };


    int idx = 0;

    if (count == 1) {
        for (auto & plane : planes) {
            float d = dot(out[0].position, plane);
            if (d >= 0) {
                return in;
            }
        }
        return out;
    }
    //线段不是封闭图形，做一次裁剪就行
    if (count == 2) {
        for (auto & plane : planes) {
            std::swap(in, out);
            if (in.empty()) {
                break;
            }
            out.clear();

            float da = dot(in[0].position, plane);
            float db = dot(in[1].position, plane);
            int code = ((da < 0.0) <<1)| (db < 0.0);
            //无论怎么样每次都产生两个否则就剔除了
            if (code == 3) {
                std::swap(in, out);
            }else if (code == 1) {
                V2f clip = lerp(in[0], in[1], da/(da-db));
                out.push_back(clip);
                out.push_back(in[1]);
            }else if (code == 2) {
                V2f clip = lerp(in[0], in[1], da/(da-db));
                out.push_back(in[0]);
                out.push_back(clip);
            }else {
                break;
            }
        }
        return out;
    }

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
            int code = (di < 0.0) | ((pred < 0.0)<<1);
            switch(code) {
                case 0: //都不在
                    break;
                case 1: //j不在 i在
                    {
                        V2f clip = lerp(in[pre], in[i], pred/(pred-di));
                        out.push_back(clip);
                        out.push_back(in[i]);
                    }
                    break;
                case 2: //j在 i不在
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

bool SGLPipeline::testDepth(int x, int y, float depth)
{
    float depthBuf = currentFrame->getDepth(x, y);
    return depthBuf > depth;
}

void SGLPipeline::writeFragment(int x, int y, float depth, const Vec4f & color)
{
    currentFrame->setDepth(x, y, depth);
    currentFrame->setPixel(x, y, color);
}

void SGLPipeline::drawPoint(const V2f & point)
{
    std::vector<V2f> in = {point};
    auto res = SutherlandHodgeman(in, 1);
    if (res.empty()) {
        return;
    }
    
    auto dp = res[0].position/res[0].position.w;
    auto sp = viewPortTrans(dp);
    float depth = dp.z;
    if (true or testDepth(sp.x, sp.y, depth)) {
        Vec4f color;
        shader->onFragment(res[0], color);
        writeFragment(sp.x, sp.y, depth, color);
    }
}

void SGLPipeline::drawLine(std::vector<V2f> & points)
{
    std::vector<V2f> clips = SutherlandHodgeman(points, 2);
    if (clips.size()) {
        auto & a = clips[0];
        auto & b = clips[1];
        auto ca = clips[0].position/clips[0].position.w;
        auto cb = clips[1].position/clips[1].position.w;

        auto A = viewPortTrans(ca);
        auto B = viewPortTrans(cb);

        int dx = B.x - A.x;
        int dy = B.y - A.y;
        bool steep = abs(dy) > abs(dx);

        if (steep) {
            std::swap(dx, dy);
            std::swap(A.x, A.y);
            std::swap(B.x, B.y);
        }
        if (A.x > B.x) {
            std::swap(A, B);
            std::swap(a, b);
            std::swap(ca, cb);
        }

        float k = abs(float(dy)/dx);
        int ystep = B.y > A.y ? 1:-1;
        float err = 0; 
        while (A.x != B.x + 1) {
            int x = A.x, y = A.y;
            if (steep) {
                std::swap(x, y);
            }

            float right = abs(float(B.x - A.x)/dx);
            V2f f = lerp(a, b, 1-right);

            Vec4f color;
            float depth = lerp(ca.z, cb.z, 1-right);
            if (testDepth(x, y, depth)) {
                shader->onFragment(f, color);
                writeFragment(x, y, depth, color);
            }

            A.x ++;
            err += k;
            if (err > 0.5) {
                A.y += ystep;
                err -= 1;
            }
        }
    }
}

void SGLPipeline::drawTriangle0(std::vector<V2f> & points)
{
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
}

void SGLPipeline::drawTriangle(const V2f & a, const V2f & b, const V2f & c)
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


            if (s1+s2+s3 <= S) {
                if (testDepth(x, y, depth)) {
                    Vec4f color;
                    shader->onFragment(f, color);
                    writeFragment(x, y, depth, color);
                }
            }		
        }
    }
}

bool SGLPipeline::backCulling(const std::vector<V2f> & points)
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

void SGLPipeline::drawArray(const Vertex * verties, size_t count, DrawMode drawMode)
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
        out.norm = v.normal;
        shader->onVertex(&v, out);

        if (drawMode == DrawMode::POINT) {
            drawPoint(out);
        } else {
            points.push_back(out);
            if ( drawMode == DrawMode::TRIANGLE && points.size() == 3) {
                drawTriangle0(points);
                points.clear();
            }else if (drawMode == DrawMode::LINE && points.size() == 2) {
                drawLine(points);
                points.clear();
            }
        }

    }
}

void SGLPipeline::drawElements(const void * verties, uint32_t stride, size_t count, unsigned int * indices, size_t indiesCount, DrawMode drawMode)
{
    if (!verties || !indices || !currentFrame || !shader) return;
    
    std::vector<V2f> points;
    for (int i = 0; i < count; ++i) {
        Vertex * vp = (Vertex*)((char*)verties + stride*i);
        V2f out;
        out.position = vp->position;
        out.color = vp->color;
        out.uv = vp->uv;
        out.norm = vp->normal;
        shader->onVertex(vp, out);
        points.push_back(out);
    }

    for (int i = 0; i < indiesCount; ++i) {
        if (drawMode == DrawMode::POINT) {
            drawPoint(points[indices[i]]);
        } else {
            if ( drawMode == DrawMode::TRIANGLE && (i+1)%3 == 0) {
                std::vector<V2f> temp = {
                    points[indices[i-2]], 
                    points[indices[i-1]], 
                    points[indices[i]], 
                };
                drawTriangle0(temp);
            }else if (drawMode == DrawMode::LINE && (i+1)%2 == 0) {
                std::vector<V2f> temp = {
                    points[indices[i-1]], 
                    points[indices[i]], 
                };
                drawLine(temp);
            }
        }
    }
}

