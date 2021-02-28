#include "maths.h"
#include <cstdio>
#include "debug.h"

int main ()
{
	Mat4f movex = 
	{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{1, 0, 0, 1}
	};
	
	Mat4f movey = 
	{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, -1, 0, 1}
	};

	//令人巴适的行向量

	//透视矩阵
	auto proj = perspective(90, 1, 1.f, 10.f);
	print(proj);

    printf("project test\n");
    Vec4f pos = {0, 0, 2, 1};
    pos = pos * proj;
    print(pos, 4);
    pos.x /= pos.w;
    pos.y /= pos.w;
    pos.z /= pos.w;
    print(pos, 4);
    printf("end\n");

    printf("view test\n");
	Vec3f at(0, 0, -1);
	Vec3f up(0, 1, 0);
	Vec3f to(0, 0, 0);
    auto view = lookat(at, to, up);
    pos = {0, 0, 2, 1};
    pos = pos * view;
    print(pos, 4);
    printf("end\n");

    printf("math %.2f\n", radians(180.f));


	auto front = normalize(to-at);
	auto right = normalize(cross(up, front));
	auto cup = normalize(cross(front, right));
	print(right, 3);
	print(cup, 3);
	print(front, 3);
	printf("inverse test\n");

	Mat4f t = 
	{
		{-4.f,-3.f,3.f,1.f},
		{0.f, 2.f,-2.f, 0.f},
		{1.f, 4.f, -1.f, 1.f},
		{1.f, 3.f, 5.f, 1.f}
	};
	print(t);
	auto i = inverse(t, 4);

}
