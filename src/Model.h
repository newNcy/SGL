#pragma once
#include <string>
#include <vector>
#include <memory>
#include "global.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <memory>

/*
 * 定义公共数据给shader和加载器使用
 */

struct BoundingBox3d
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
	Vec3f ambient{1,1,1}; //ka
	Vec3f diffuse{1,1,1}; //kd
	Vec3f specular{1,1,1}; //ks
	std::shared_ptr<Texture> ambientMap;
	std::shared_ptr<Texture> diffuseMap;
	std::shared_ptr<Texture> specularMap;
	std::shared_ptr<Texture> emissiveMap;
	std::shared_ptr<Texture> normalMap;
};

struct Mesh
{
	std::string name;
	std::vector<Vertex> verties;
	std::vector<uint32_t> indices;
	std::shared_ptr<Material> material;
};

struct Model
{
	BoundingBox3d boundingBox;
	std::vector<std::shared_ptr<Mesh>> meshs;
};

struct SkinnedMesh
{
    std::vector<SkinnedVertex> vertices;
	std::vector<uint32_t> indices;
	std::shared_ptr<Material> material;
};

struct SkinnedModel
{
    std::string path;
	BoundingBox3d boundingBox;
	std::vector<SkinnedMesh> meshes;
    void processNode(aiNode * node, const aiScene * scene);
    SkinnedMesh loadMesh(aiMesh * mesh, const aiScene * scene);
    bool load(const std::string & path);
};


