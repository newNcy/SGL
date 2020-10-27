#pragma once
#include "maths.h"
#include <cstdio>

template <typename V>
void print(const V & v, int d = 2, char end = '\n')
{
	printf("{%f", v[0]);
	for (int i = 1; i < d; ++ i) {
		printf(", %f", v[i]);
	}
	printf("}%c", end);
	fflush(stdout);
}

void print(const Mat4f & m, char end = '\n');

