#include "debug.h"
int Profile::tab = 0;
bool Profile::clear = false;

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
