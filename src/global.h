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
    Vec4i boneId = {-1, -1, -1, -1};
    Vec4f weights;
    void bindToBone(uint32_t idx, float weight)
    {
        for (int i = 0 ; i < 4; ++ i) {
            if (weights[i] == 0.0) {
                boneId[i] = idx;
                weights[i] = weight;
                return;
            }
        }
    }
};


