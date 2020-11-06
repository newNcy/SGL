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

	Mat4f combine = movex * movey;
	printf("translate combine");
	print(combine);
	Vec4f pos = {2, 3, 4, 1};
	print(pos, 4);
	//令人巴适的行向量

	//透视矩阵
	printf("project");
	auto proj = perspective(90, 1, 0.1f, 1000.f);
	print(proj);

	Vec4f pos2 = {2.5,2.5,2.5, 1};
	print(pos2, 4);
	auto prop = pos2 * proj;
	print(prop, 4);
	print(prop/prop.w, 4);
	printf("glm\n");
	Vec3f at(0, 0, -1);
	Vec3f up(0, 1, 1);
	Vec3f to(0, 0, 0);

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
