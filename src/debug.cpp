#include "debug.h"
int Profiler::tab = 0;
Profiler * Profiler::firstNode= nullptr;
std::vector<ProfileEntry> Profiler::hits;

void print(const Mat4f & m, char end)
{
	printf("{\n");
	for (int i = 0 ; i < 4; ++ i) {
		printf("\t");
		print(m[i], 4);
	}
	printf("}%c", end);
	fflush(stdout);
}
