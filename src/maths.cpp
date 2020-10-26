#include "maths.h"
#include <algorithm>
#include "debug.h"

Mat4f lookat(const Vec3f & pos, const Vec3f & target, const Vec3f & up)
{
	auto front = normalize(target - pos);
	auto right = normalize(cross(front, up));
	auto camup = normalize(cross(right, front));

	print(front, 3);
	print(right, 3);
	print(camup, 3);
	Mat4f view = 
	{
		Vec4f(right, -pos.x),
		Vec4f(camup, -pos.y),
		Vec4f(front, -pos.z),
		Vec4f(0,0, 0, 1)
	};

	for (int i = 0 ; i < 4; ++i ){
		for (int j = 0 ; j < 4; ++j ){
			std::swap(view[i][j], view[j][i]);
		}
	}
	return view;
}

Mat4f perspective(float fov,float aspect, float n, float f)
{
	fov = int(fov)%180/180.f*3.1415926/2;
	float h = n*tan(fov);
	float w = h*aspect;
	printf("%f %f\n", w, h);
	Mat4f projection = 
	{
		{2*n/w, 0,			0,				0},
		{0,		2*n/h,		0,				0},
		{0,		0,			(f+n)/(f-n),	1},
		{0,		0,			-2*f*n/(f-n),	0},
	};
	return projection;
}
