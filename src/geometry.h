#pragma once 
#include "maths.h"
#include "Pipeline.h"
#include <vector>


/*
 * 生成各种形状的物件(好生成的话)
 */

std::vector<Vertex> genCube()
{
	std::vector<Vertex> ret = 
	{
		//前
		{{-1.f, 1.f, -1.f}, {},{0,1}, {0.f,0.f,-1.f}},
		{{ 1.f, 1.f, -1.f}, {},{1,1}, {0.f,0.f,-1.f}},
		{{ 1.f,-1.f, -1.f}, {},{1,0}, {0.f,0.f,-1.f}},
		{{-1.f, 1.f, -1.f}, {},{0,1}, {0.f,0.f,-1.f}},
		{{-1.f,-1.f, -1.f}, {},{0,0}, {0.f,0.f,-1.f}},
		{{ 1.f,-1.f, -1.f}, {},{1,0}, {0.f,0.f,-1.f}},

		//后
		{{-1.f, 1.f,  1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f,  1.f}, {},{1,1}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {},{1,0}, {0.f,0.f,0.f}},
		{{-1.f, 1.f,  1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{-1.f,-1.f,  1.f}, {},{0,0}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {},{1,0}, {0.f,0.f,0.f}},
	
		//左
		{{-1.f, 1.f,  1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{-1.f, 1.f, -1.f}, {},{1,1}, {0.f,0.f,0.f}},
		{{-1.f,-1.f, -1.f}, {},{1,0}, {0.f,0.f,0.f}},
		{{-1.f, 1.f,  1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{-1.f,-1.f,  1.f}, {},{0,0}, {0.f,0.f,0.f}},
		{{-1.f,-1.f, -1.f}, {},{1,0}, {0.f,0.f,0.f}},

		//右
		{{ 1.f, 1.f, -1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f,  1.f}, {},{1,1}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {},{1,0}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f, -1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f, -1.f}, {},{0,0}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {},{1,0}, {0.f,0.f,0.f}},

		//上
		{{-1.f, 1.f,  1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f,  1.f}, {},{1,1}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f, -1.f}, {},{1,0}, {0.f,0.f,0.f}},
		{{-1.f, 1.f,  1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{-1.f, 1.f, -1.f}, {},{0,0}, {0.f,0.f,0.f}},
		{{ 1.f, 1.f, -1.f}, {},{1,0}, {0.f,0.f,0.f}},

		//下
		{{-1.f,-1.f, -1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f, -1.f}, {},{1,1}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {},{1,0}, {0.f,0.f,0.f}},
		{{-1.f,-1.f, -1.f}, {},{0,1}, {0.f,0.f,0.f}},
		{{-1.f,-1.f,  1.f}, {},{0,0}, {0.f,0.f,0.f}},
		{{ 1.f,-1.f,  1.f}, {},{1,0}, {0.f,0.f,0.f}},
	};
	return ret;
}

std::vector<Vertex> genSphare(float radius)
{
	std::vector<Vertex> points;
	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	const float PI = 3.14159265359;
	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			Vertex v;
			v.position = Vec3f(xPos, yPos, zPos);
			v.color = Vec3f(0.6f, 0.6f, 0.6f);//Vec3f((xPos+1.0)*0.5f, (yPos+1.0)*0.5f, (zPos+1.0)*0.5f);
			int last = points.size() -1;
			points.push_back(v);
		}
	}

	std::vector<Vertex> ret;

	for (int j = 0; j < X_SEGMENTS; ++ j) {
		for (int i = 1; i <= Y_SEGMENTS; ++ i) {
			int idx = j*X_SEGMENTS + i;
			ret.push_back(points[idx]);
			if (i % 2 == 0) {
				ret.push_back(points[idx-1]);
				ret.push_back(points[idx+X_SEGMENTS]);
				ret.push_back(points[idx+X_SEGMENTS]);
				ret.push_back(points[idx+1]);
			}else {
				ret.push_back(points[idx+X_SEGMENTS-1]);
				ret.push_back(points[idx+X_SEGMENTS]);
				ret.push_back(points[idx+X_SEGMENTS]);
				ret.push_back(points[idx+X_SEGMENTS+1]);
			}
			ret.push_back(points[idx]);
		}
	}
	return ret;
}

std::vector<Vertex> genGround(int width, int height)
{
	return 
	{
		{{0, 0, 0},{0.3f, 0.3f, 0.3f}, {0,0}, {}},
		{{0, 0, height}, {0.3f, 0.3f, 0.3f}, {0,1},{}},
		{{width, 0, height}, {0.3f, 0.3f, 0.3f}, {1,1},{}},

		{{0, 0, 0}, {0.3f, 0.3f, 0.3f}, {0,0}, {}},
		{{width, 0, 0}, {0.3f, 0.3f, 0.3f}, {1,0},{}},
		{{width, 0, height}, {0.3f, 0.3f, 0.3f}, {1,1},{}},
	};
}

