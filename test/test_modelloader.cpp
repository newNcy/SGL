#include "ModelLoader.h"
#include "debug.h"
#include <memory>
int main ()
{
	auto loader = std::make_shared<OBJLoader>();
	auto model = loader->load("../../resource/model/nanosuit/nanosuit.obj");
	return 0;
}
