#pragma once
#include <cassert>
#include <cmath>
#include <stdio.h>

const static float pi = 3.1415926;
inline float radians(float an) { return an/180*pi; }
inline float halfRadians(float an) { return an/360*pi; }

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
	Type operator * (const Type & rhs) const { return Type(x*rhs.x, y*rhs.y); }
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
	Type operator * (const Type & rhs) const { return Type(x*rhs.x, y*rhs.y, z*rhs.z); }
	Type operator * (float f) const { return Type(x*f, y*f, z*f); }
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
struct Quaternion;

template <typename T>
struct Mat4
{
	using Vec4 = Vec4<T>;
	using Type = Mat4<T>;
	using Etype = T;
    static Type identify;
	Vec4 data[4];

    Mat4(const Quaternion<T> & quat); 
	Mat4() {data[0][0] = 1; data[1][1] = 1; data[2][2] = 1; data[3][3] = 1;}
	Mat4(const Vec4 & a, const Vec4 & b, const Vec4 & c, const Vec4 & d):data{a, b, c, d} {}
	Type operator + (const Type & rhs) const { return Type(data[0]+rhs[0], data[1]+rhs[1], data[2]+rhs[2], data[3]+rhs[3]); }
	Type operator - (const Type & rhs) const { return Type(data[0]-rhs[0], data[1]-rhs[1], data[2]-rhs[2], data[3]-rhs[3]); }
	Type operator * (float f) const { return Type(data[0]*f, data[1]*f, data[2]*f, data[3]*f); }
	Type operator / (float f) const { return Type(data[0]/f, data[1]/f, data[2]/f, data[3]/f); }

	Vec4 & operator [] (unsigned int index) { assert(index < 4); return data[index];}
	const Vec4 & operator [] (unsigned int index) const { assert(index < 4); return data[index];}
};

template <typename T>
struct Quaternion
{
	using Type = Quaternion<T>;
	using Etype = T;
	union 
	{
		struct { T x,y,z,w; };
		struct { T r,g,b,a = 1; };
		T data[4];
	};
	Quaternion():x(0), y(0),z(0),w(0) {}
	Quaternion(T x, T y, T z, T w):x(x), y(y), z(z),w(w) {}
	Quaternion(const Type & rhs):x(rhs.x),y(rhs.y),z(rhs.z),w(rhs.w) {}
	Quaternion(const Vec3<T> & axis, float theta) 
    { 
        float sinTheata = sin(theta);
        auto n = axis * sinTheata;
        w = cos(theta);
        x = n.x;
        y = n.y;
        z = n.z;
    }

    Type inverse() const { return conjugate()/length(); }
	Type conjugate() const { return Type(-x, -y, -z, w); }
    T length() const { return sqrt(x*x+ y*y+ z*z+ w*w); }


	Type operator + (const Type & rhs) const { return Type(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w); }
	Type operator - (const Type & rhs) const { return Type(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w); }
	Type operator * (float f) const { return Type(x*f, y*f, z*f, w*f); }	
	Type operator / (float f) const { return Type(x/f, y/f, z/f, w/f); }
	T & operator [] (unsigned int index) { assert(index < 4); return data[index];}
	//T operator * (const Type & rhs) const { return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w; }
	const T & operator [] (unsigned int index) const { assert(index < 4); return data[index];}
};

template <typename T>
Mat4<T>::Mat4(const Quaternion<T> & quat)
{
    *this={
        {1.f - 2*quat.y*quat.y -2*quat.z*quat.z, 2*quat.x*quat.y+2*quat.w*quat.z,  2*quat.x*quat.z-2*quat.w*quat.y, 0},
        {2*quat.x*quat.y -2*quat.w*quat.z, 1-2*quat.x*quat.x-2*quat.z*quat.z,  2*quat.y*quat.z+ 2*quat.w*quat.x, 0},
        {2*quat.x*quat.y +2*quat.w*quat.y, 2*quat.y*quat.z-2*quat.w*quat.x,  1-2*quat.x*quat.x-2*quat.y*quat.y, 0},
        {0,0,0,1}
    };
}

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


/**
 * 跟标准定义后面的叉乘部分相反的，使得ad = b,计算旋转跟矩阵一样往右乘
 */
template <typename T>
Quaternion<T> cross (const Quaternion<T> & lhs,const Quaternion<T> & rhs)
{ 
    return Quaternion<T>(
            lhs.w*rhs.x + lhs.x*rhs.w + rhs.y*lhs.z - rhs.z*lhs.y,
            lhs.w*rhs.y + lhs.y*rhs.w + rhs.z*lhs.x - rhs.x*lhs.z,
            lhs.w*rhs.z + lhs.z*rhs.w + rhs.x*lhs.y - rhs.y*lhs.x,

            lhs.w*rhs.w - lhs.x*rhs.x - lhs.y*rhs.y - lhs.z*rhs.z
            ); 
}

template <typename T>
Quaternion<T> delta(const Quaternion<T> & lhs,const Quaternion<T> & rhs)
{
    return cross(lhs.inverse(), rhs);
}

template <typename T>
Quaternion<T> log(const Quaternion<T> & quat)
{
    return Quaternion<T>(asin(quat.x), asin(quat.y), asin(quat.z), 0);
}


template <typename T>
T dot(const Vec4<T> & lhs, const Vec4<T> & rhs)
{
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
}

template <typename T>
T dot(const Quaternion<T> & lhs, const Quaternion<T> & rhs)
{
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
}

template <typename T>
Quaternion<T> lerp(const Quaternion<T> & lhs, const Quaternion<T> & rhs, float t)
{
    auto rhs2 = rhs;
    float cosOmega = dot(lhs, rhs2);
    if (cosOmega < 0.f) {
        cosOmega *= -1;
        rhs2 = rhs2 * -1;
    }

    float k0, k1;
    if (cosOmega > .999f) {
        k0 = 1-t;
        k1 = t;
    } else {
        float sinOmega = sqrt(1- cosOmega*cosOmega);
        float omega = atan2(sinOmega, cosOmega)/pi*180;
        float oneOverSinOmega = 1.f/sinOmega;
        k0 = sin(radians((1.f-t)*omega)) * oneOverSinOmega;
        k1 = sin(radians(t*omega)) * oneOverSinOmega;
    }

    return lhs * k0 + rhs2*k1;
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
using Vec4i = Vec4<int>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;
using Mat4f = Mat4<float>;
using Quat = Quaternion<float>;

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

Mat4f inverse(const Mat4f & m);

