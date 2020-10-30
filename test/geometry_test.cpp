#include "Pipeline.h"
#include "geometry.h"
#include "debug.h"


int main ()
{
	auto sphare = genSphare(1.0f);
	for (auto & p : sphare) {
		print(p.position, 3);
	}
}
