#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

Texture::Texture(const char * path)
{
	data = stbi_load(path, &width, &height, &n, 0);
	assert(data);
}

Vec4f Texture::sample(float u, float v)
{
	int x = u*(width-1);
	int y = (1-v)*(height-1);
	unsigned char * p = data + y*n*width + x*n;
	Vec4f ret(p[0]/255.f, p[1]/255.f,p[2]/255.f, 1.f);
	return ret;
}

Texture::~Texture()
{
	stbi_image_free(data);
}
