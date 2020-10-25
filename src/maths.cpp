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

Mat4f perspective(float fov, float n, float f)
{
	Mat4f projection;
	return projection;
}
