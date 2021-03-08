#include "maths.h"
#include <algorithm>
#include "debug.h"

template <>
Mat4f Mat4f::identify = 
{
    {1, 0,0,0},
    {0, 1,0,0},
    {0, 0,1,0},
    {0, 0,0,1},
};


Mat4f lookat(const Vec3f & pos, const Vec3f & target, const Vec3f & up)
{
	auto z = normalize(pos - target);
	auto x = normalize(cross(up, z));
	auto y = normalize(cross(z, x));

	Mat4f view = 
	{
		Vec4f(x, 0),
		Vec4f(y, 0),
		Vec4f(z, 0),
		Vec4f(0, 0, 0, 1)
	};
	//在还是正交阵的时候转置取逆
	view = transpose(view, 3);

	Mat4f move = 
	{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		Vec4f(pos*-1, 1)
	};
	return move * view;
}

Mat4f perspective(float fov,float aspect, float n, float f)
{
	fov = int(fov)%180/180.f*3.1415926/2;
	float h = 2*n*tan(fov);
	float w = h*aspect;
	Mat4f projection = 
	{
		{-2*n/w, 0,			0,				0},
		{0,		-2*n/h,		0,				0},
		{0,		0,			(f+n)/(f-n),	1},
		{0,		0,			2*f*n/(f-n),	0},
	};
	return projection;
}

Mat4f moveto(float x, float y, float z)
{
	return Mat4f 
	{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		Vec4f(x, y, z, 1)
	};
}

Mat4f rotate(float x, float y, float z)
{
	x = radians(x);
	y = radians(y);
	z = radians(z);
	Mat4f rx = 
	{
		{1, 0, 0, 0,},
		{0, cos(x),  sin(x), 0},
		{0, -sin(x), cos(x), 0},
		{0, 0, 0, 1}
	};

	Mat4f ry = 
	{
		{cos(y), 0, -sin(y), 0},
		{0, 1, 0, 0},
		{sin(y), 0, cos(y), 0},
		{0, 0, 0, 1}
	};

	Mat4f rz = 
	{
		{cos(z), sin(z), 0, 0},
		{-sin(z), cos(z), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	return rx * ry * rz;
}

Mat4f scale(float x, float y, float z)
{
	return Mat4f 
	{
		{x, 0, 0, 0 },
		{0, y, 0, 0},
		{0, 0, z, 0},
		{0, 0, 0, 1}
	};
}
