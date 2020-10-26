#pragma once
#include <cassert>
#include <cmath>
#include <stdio.h>

template <typename T>
struct Vec2
{
	using Type = Vec2<T>;
	union 
	{
		struct { T x,y; };
		struct { T u,v; };
		T data[2];
	};
	Vec2():x(0), y(0) {}
	Vec2(T x, T y):x(x), y(y) {}

	Type operator + (const Type & rhs) const { return Type(x+rhs.x, y+rhs.y); }
	Type operator - (const Type & rhs) const { return Type(x-rhs.x, y-rhs.y); }
	Type operator * (float f) const { return Type(x*f, y*f); }
	Type operator / (float f) const { return Type(x/f, y/f); }
	T & operator [] (unsigned int index) { assert(index < 2); return data[index];}
	const T & operator [] (unsigned int index) const { assert(index < 2); return data[index];}
};


template <typename T>
struct Vec3
{
	using Type = Vec3<T>;
	union 
	{
		struct { T x,y,z; };
		struct { T r,g,b; };
		T data[3];
	};
	Vec3():x(0), y(0),z(0) {}
	Vec3(T x, T y, T z):x(x), y(y), z(z) {}

	Type operator + (const Type & rhs) const { return Type(x+rhs.x, y+rhs.y, z+rhs.z); }
	Type operator - (const Type & rhs) const { return Type(x-rhs.x, y-rhs.y, z-rhs.z); }
	Type operator * (float f) const { return Type(x*f, y*f, z*f); }
	Type operator / (float f) const { return Type(x/f, y/f, z/f); }
	T & operator [] (unsigned int index) { assert(index < 3); return data[index];}
	const T & operator [] (unsigned int index) const { assert(index < 3); return data[index];}
};

template <typename T>
struct Vec4
{
	using Type = Vec4<T>;
	union 
	{
		struct { T x,y,z,w; };
		struct { T r,g,b,a; };
		T data[4];
	};
	Vec4():x(0), y(0),z(0),w(0) {}
	Vec4(T x, T y, T z, T w):x(x), y(y), z(z),w(w) {}
	Vec4(const Vec3<T> & v3, T w = 0):x(v3.x), y(v3.y),z(v3.z),w(w) {}

	Type operator + (const Type & rhs) const { return Type(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w); }
	Type operator - (const Type & rhs) const { return Type(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w); }
	Type operator * (float f) const { return Type(x*f, y*f, z*f, w*f); }
	Type operator / (float f) const { return Type(x/f, y/f, z/f, w/f); }
	T & operator [] (unsigned int index) { assert(index < 4); return data[index];}
	const T & operator [] (unsigned int index) const { assert(index < 4); return data[index];}
};

template <typename T>
struct Mat4
{
	using Vec4 = Vec4<T>;
	using Type = Mat4<T>;
	Vec4 data[4];

	Mat4() {}
	Mat4(const Vec4 & a, const Vec4 & b, const Vec4 & c, const Vec4 & d):data{a, b, c, d} {}
	Type operator + (const Type & rhs) const { return Type(*this[0]+rhs[0], *this[1]+rhs[1], *this[2]+rhs[2], *this[3]+rhs[3]); }
	Type operator - (const Type & rhs) const { return Type(*this[0]-rhs[0], *this[1]-rhs[1], *this[2]-rhs[2], *this[3]-rhs[3]); }
	Type operator * (float f) const { return Type(*this[0]*f, *this[1]*f, *this[2]*f, *this[3]*f); }
	Type operator / (float f) const { return Type(*this[0]/f, *this[1]/f, *this[2]/f, *this[3]/f); }

	Vec4 & operator [] (unsigned int index) { assert(index < 4); return data[index];}
	const Vec4 & operator [] (unsigned int index) const { assert(index < 4); return data[index];}
};


template <typename T>
Vec3<T> cross(const Vec3<T> & lhs, const Vec3<T> & rhs)
{
	return 
	{
		lhs.y*rhs.z - lhs.z*rhs.y,
		lhs.z*rhs.x - lhs.x*rhs.z,
		lhs.x*rhs.y - lhs.y*rhs.x
	};
}

template <typename T>
Vec3<T> normalize(const Vec3<T> & v3)
{
	return v3/sqrt(v3.x*v3.x + v3.y*v3.y + v3.z*v3.z);
}

template <typename T>
Mat4<T> operator * (const Mat4<T> & lhs, const Mat4<T> & rhs)
{
	Mat4<T> ret;
	for (int i = 0 ; i < 4; ++ i) {
		for (int j = 0 ; j < 4; ++ j) {
			ret[i][j] = 0;
			for (int k = 0 ; k < 4; ++ k) {
				ret[i][j] += lhs[i][k]*rhs[k][j];
			}
		}
	}
	return ret;
}

template <typename T>
Vec4<T> operator * (const Vec4<T> & vec, const Mat4<T> & mat)
{
	Vec4<T> ret;
	for (int i = 0 ; i < 4; ++ i) {
		ret[i] = 0;
		for (int j = 0 ; j < 4; ++ j) {
			ret[i] += vec[j]*mat[j][i];
			printf("v[%d] += %f\n", i, vec[j]*mat[j][i]);
		}
	}
	return ret;
}

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;
using Mat4f = Mat4<float>;

Mat4f lookat(const Vec3f & pos, const Vec3f & target, const Vec3f & up);
Mat4f perspective(float fov, float aspect, float n, float f);
