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
	print(combine);
	Vec4f pos = {2, 3, 4, 1};
	print(pos, 4);
	//令人巴适的行向量

	//透视矩阵
	auto proj = perspective(90, 1, 1, 2);
	print(proj);

	Vec4f pos2 = {1,0.5,1, 1};
	print(pos2, 4);
	printf("%f\n", tan(3.1415926/4));
	auto prop = pos2 * proj;
	print(prop, 4);
	print(prop/prop.w, 4);

	printf("camera test\n");
	Vec3f cpos(0, 0, -1);
	Vec3f target(0, 0, 0);
	Vec3f up(0, 1, 0);
	auto  view = lookat(cpos, target, up);
	print(view);
}
