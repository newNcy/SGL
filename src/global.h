#pragma once
#include "maths.h"

struct Vertex
{
	Vec3f position;	//位置
	Vec3f color;	//颜色
	Vec2f uv;		//纹理坐标
	Vec3f normal;		//法向量
};

struct SkinnedVertex : public Vertex
{
    Vec4i boneId;
    Vec4f weights;
};


