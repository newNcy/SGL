#pragma once
#include <cassert>

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

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
