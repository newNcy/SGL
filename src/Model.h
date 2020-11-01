#pragma once
#include <string>
#include <vector>
#include <memory>
#include "global.h"

/*
 * 定义公共数据给shader和加载器使用
 */

struct BoudingBox3d
{
	Vec3f min;
	Vec3f max;
};

struct Texture
{
	Texture(const char * path);
	Vec4f sample(float u, float v);
	~Texture();

	int width = 0;
	int height = 0;
	int n = 0;
	unsigned char * data = nullptr;
};

struct Material
{
	std::string name;
	Vec3f ambient; //ka
	Vec3f diffuse; //kd
	Vec3f specular; //ks
	std::shared_ptr<Texture> ambientMap;
	std::shared_ptr<Texture> diffuseMap;
	std::shared_ptr<Texture> specularMap;
	std::shared_ptr<Texture> emissiveMap;
};

struct Mesh
{
	std::string name;
	std::vector<Vertex> verties;
	std::shared_ptr<Material> material;
};

struct Model
{
	BoudingBox3d boundingBox;
	std::vector<std::shared_ptr<Mesh>> meshs;
};

