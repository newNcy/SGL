#pragma once

template <unsigned D>
struct XYZW {};


template <typename T, unsigned D>
struct Vec2
{
	union 
	{
		struct { T u,v; };
		T data[2];
	};
};
