#pragma once
#include <cassert>
#include <cmath>
#include <stdio.h>


template <typename T>
struct Vec2
{
	using Type = Vec2<T>;
	using Etype = T;
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
	using Etype = T;
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
	Type operator * (const Type & rhs) const { return Type(x*rhs.x, y*rhs.y, z*rhs.z); }
	Type operator / (float f) const { return Type(x/f, y/f, z/f); }
	T & operator [] (unsigned int index) { assert(index < 3); return data[index];}
	const T & operator [] (unsigned int index) const { assert(index < 3); return data[index];}
};

template <typename T>
struct Vec4
{
	using Type = Vec4<T>;
	using Etype = T;
	union 
	{
		struct { T x,y,z,w; };
		struct { T r,g,b,a = 1; };
		T data[4];
	};
	Vec4():x(0), y(0),z(0),w(0) {}
	Vec4(T x, T y, T z, T w):x(x), y(y), z(z),w(w) {}
	Vec4(const Vec3<T> & v3, T w = 0):x(v3.x), y(v3.y),z(v3.z),w(w) {}

	Type operator + (const Type & rhs) const { return Type(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w); }
	Type operator - (const Type & rhs) const { return Type(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w); }
	Type operator * (float f) const { return Type(x*f, y*f, z*f, w*f); }
	Type operator * (const Type & rhs) const { return Type(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w); }
	Type operator / (float f) const { return Type(x/f, y/f, z/f, w/f); }
	T & operator [] (unsigned int index) { assert(index < 4); return data[index];}
	//T operator * (const Type & rhs) const { return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w; }
	const T & operator [] (unsigned int index) const { assert(index < 4); return data[index];}
};

template <typename T>
struct Mat4
{
	using Vec4 = Vec4<T>;
	using Type = Mat4<T>;
	using Etype = T;
	Vec4 data[4];

	Mat4() {data[0][0] = 1; data[1][1] = 1; data[2][2] = 1; data[3][3] = 1;}
	Mat4(const Vec4 & a, const Vec4 & b, const Vec4 & c, const Vec4 & d):data{a, b, c, d} {}
	Type operator + (const Type & rhs) const { return Type(*this[0]+rhs[0], *this[1]+rhs[1], *this[2]+rhs[2], *this[3]+rhs[3]); }
	Type operator - (const Type & rhs) const { return Type(*this[0]-rhs[0], *this[1]-rhs[1], *this[2]-rhs[2], *this[3]-rhs[3]); }
	Type operator * (float f) const { return Type(*this[0]*f, *this[1]*f, *this[2]*f, *this[3]*f); }
	Type operator / (float f) const { return Type(*this[0]/f, *this[1]/f, *this[2]/f, *this[3]/f); }

	Vec4 & operator [] (unsigned int index) { assert(index < 4); return data[index];}
	const Vec4 & operator [] (unsigned int index) const { assert(index < 4); return data[index];}
};

template <typename T>
T lerp(const T & start, const T & end, float factor)
{
	return start*(1.f-factor) + end*factor;
}

template <typename T>
T dot(const Vec3<T> & lhs, const Vec3<T> & rhs)
{
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

template <typename T>
T dot(const Vec4<T> & lhs, const Vec4<T> & rhs)
{
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
}

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
	float length = v3.x*v3.x + v3.y*v3.y + v3.z*v3.z;
	//print(v3, 3);
	return v3/sqrt(length);
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

const static float pi = 4.1415926;
inline float radians(float an) { return an/180*pi; }
Mat4f lookat(const Vec3f & pos, const Vec3f & target, const Vec3f & up);
Mat4f perspective(float fov, float aspect, float n, float f);

Mat4f moveto(float x, float y, float z);
Mat4f rotate(float x, float y, float z);
Mat4f scale(float x, float y, float z);

template <typename T>
T transpose(const T & t, int d)
{
	T ret;
	for (int i = 0; i < d; ++ i) {
		for (int j = 0; j < d; ++ j) {
			ret[i][j] = t[j][i];
		}
	}
	return ret;
}

template <typename T>
T inverse(const T & m, int d)
{
	printf("%s\n", __PRETTY_FUNCTION__);
	typename T::Etype det = 0;
	for (int i = 0 ; i < d; ++ i) {
		typename T::Etype add = m[0][i];
		typename T::Etype sub = m[0][i];
		for (int j = 1 ; j < d; ++ j) {
			int r1 = (i+j)%d;
			int r2 = (i-j+d)%d;
			printf("add[%d][%d]:%fx%f\n", j, r1, add, m[j][r1]);
			printf("sub[%d][%d]:%fx%f\n", j, r2, sub, m[j][r2]);
			add *= m[j][r1];
			sub *= m[j][r2];
		}
		printf("\n");
		det += add - sub;
	}
	printf("det :%f\n", det);

	float det2 = 0;
	return m;
}


