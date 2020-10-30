#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

Texture::Texture(const char * path)
{
	data = stbi_load(path, &width, &height, &n, 0);
}

Vec4f Texture::sample(float u, float v)
{
	int x = u*(width-1);
	int y = (1-v)*(height-1);
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= width) x = width -1;
	if (y >= height) y = height-1;
	unsigned char * p = data + y*n*width + x*n;
	Vec4f ret(p[0]/255.f, p[1]/255.f,p[2]/255.f, 1.f);
	return ret;
}

Texture::~Texture()
{
	stbi_image_free(data);
}
