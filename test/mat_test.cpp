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

    Vec4f a = {1, 0, -2, 1};
    Vec4f b = {0, 1, -2, 1};
    Vec4f c = {0, 0, -1, 1};
    std::vector<Vec4f> ps = {a,b,c};
	//透视矩阵
	auto proj = perspective(90, 1, 0.1f, 5.f);
    printf("project test\n");
    for (auto & p : ps) {
        auto res = p * proj;
        printf("-----\n");
        print(p, 4);
        print(res, 4);
        res.x /= res.w;
        res.y /= res.w;
        res.z /= res.w;
        print(res, 4);
        printf("-----\n");
    }
    printf("end\n");
    Vec4f pos = a;
    pos = pos * proj;
    print(pos, 4);
    pos.x /= pos.w;
    pos.y /= pos.w;
    pos.z /= pos.w;
    print(pos, 4);

    printf("view test\n");
	Vec3f at(0, 0, 1);
	Vec3f up(0, 1, 0);
	Vec3f to(0, 0, 0);
    auto view = lookat(at, to, up);
    pos = {2, 2, -2, 1};
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

    printf("quaternion\n");
    Quat q(Vec3f(0,0,1), halfRadians(90));
    Quat q2(Vec3f(0,0,1), halfRadians(0));
    auto q3 = lerp(q2, q, 1.f/3);

    Quat p(1, 0, 0, 0);
    auto pp = cross(q3.inverse(), cross(p,q3));
    printf("%.2f, %.2f, %.2f %.2f\n", pp.x, pp.y, pp.z, pp.w);
	//print(t);
	//auto i = inverse(t, 4);

}
