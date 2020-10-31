#pragma once
#include "maths.h"
#include <cstdio>
#include <time.h>
#include <string>
#include <chrono>
#include <map>
#include <vector>

struct ProfileEntry
{
	std::string tag;
	int tab = 0;
	int count = 0;
	long long micros = 0;
};

struct Profiler
{
	static int tab;
	static Profiler * firstNode;
	static std::vector<ProfileEntry> hits;
	static void setupProfile(const char * name)
	{
		for (auto & entry : hits) {
			if (entry.tag == name) {
				entry.count ++;
				return;
			}
		}

		ProfileEntry entry =
		{
			name,
			tab,
			1,
			0
		};
		hits.push_back(entry);
	}

	static void hitProfile(const char * name,long long ms)
	{
		for (auto & entry : hits) {
			if (entry.tag == name) {
				entry.micros += ms;
				return;
			}
		}
	}

	static void show()
	{
		printf("\e[2J\e[?25l");
		printf("\e[0;0H");
		for (auto & item : hits) {
			if (!item.count) {
				continue;
			}
			int t = item.tab;
			while (t--) {
				putchar(' ');
			}
			printf("[%s] hits:%d avg:%.2lfms total:%ldms\n", 
					item.tag.c_str(),
					item.count,
					double(item.micros/1000000)/item.count,
					item.micros/1000000);
		}
		fflush(stdout);
		hits.clear();
	}

	std::chrono::system_clock::time_point start;
	const char * tag = nullptr;
	Profiler(const char * tag):tag(tag) 
	{
		if (!firstNode) {
			firstNode = this;
		}
		start = std::chrono::system_clock::now();
		setupProfile(tag);
		tab ++;
	}

	~Profiler()
	{
		tab --;
		auto end = std::chrono::system_clock::now();
		auto use = end - start;
		hitProfile(tag, use.count());
		if (this == firstNode) {
			show();
		}
	}
};
#define PROFILE(X) Profiler X##the__profiler(#X);

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

