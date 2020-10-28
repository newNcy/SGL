#pragma once
#include "maths.h"
#include <cstdio>
#include <time.h>
#include <string>
#include <chrono>

struct Profile
{
	static int tab;
	static bool clear;
	std::chrono::system_clock::time_point start;
	const char * tag = nullptr;
	Profile(const char * tag):tag(tag) 
	{
		start = std::chrono::system_clock::now();
		tab ++;
	}
	~Profile()
	{
		tab --;
		if (!clear) {
			printf("\e[2Ji\e[?25l");
			clear = true;
		}
		if (!tab) {
			printf("\e[0;0H");
		}
		auto end = std::chrono::system_clock::now();
		auto use = end - start;
		for (int i = 0 ; i < tab; ++ i) {
			printf("\t");
		}
		printf("[%s] uses %d ms\n", tag, use.count()/1000000);
		fflush(stdout);
	}
};
#define PROFILE(X) Profile X##the__profiler(#X);

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

