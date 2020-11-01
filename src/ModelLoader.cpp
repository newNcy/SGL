#include "ModelLoader.h"
#include "maths.h"
#include "debug.h"
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

std::vector<std::string> split(const char * text, const char sp)
{
	std::vector<std::string> ret;
	int len = strlen(text);
	int s = 0;
	while (s < len) {
		while (text[s] == ' ' || text[s] == sp) ++ s;
		if  (s >= len) {
			break;
		}
		int e = s+1;
		while (e < len && text[e] != sp) ++ e;
		ret.push_back(std::string(text + s, e - s));
		s = e + 1;
	}
	return ret;
}

std::string getParentPath(const std::string & path)
{
	int e = path.size() - 2;
	if (e < 0) {
		return std::string();
	}

	while (e >= 0 && path[e] != '/') -- e;
	if (e >= 0 ) {
		return path.substr(0, e+1);
	}
	return "../";
}

template <typename T>
int makeidx(int idx, const T & t)
{
	assert(idx > 0);
	if (idx > 0) return idx-1;
	return t.size() + idx;
}

void skipLine(std::ifstream & file)
{
	while (!file.eof() && file.get() != '\n');
}

std::map<std::string, std::shared_ptr<Material>> OBJLoader::loadMtl(const char * path)
{

	std::map<std::string, std::shared_ptr<Material>> ret;
	std::string dir = getParentPath(path);
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		return ret;
	}
	
	std::shared_ptr<Material> curmat;
	while (!file.eof()) {
		std::string type;
		file>>type;
		if (type[0] == '#') {
			skipLine(file);
			continue;
		} else if (type == "newmtl") {
			std::string name;
			file>>name;
			curmat = std::make_shared<Material>();
			ret[name] = curmat;
			curmat->name = std::move(name);
		} else if (type == "map_Ka") {
			if (curmat) {
				std::string mapPath;
				file>>mapPath;
				curmat->ambientMap = std::make_shared<Texture>((dir + mapPath).c_str());
			}
		} else if (type == "map_Kd") {
			if (curmat) {
				std::string mapPath;
				file>>mapPath;
				curmat->diffuseMap = std::make_shared<Texture>((dir + mapPath).c_str());
			}
		} else if (type == "map_Ks") {
			if (curmat) {
				std::string mapPath;
				file>>mapPath;
				curmat->specularMap= std::make_shared<Texture>((dir + mapPath).c_str());
			}

		} else {
			skipLine(file);
		}
	}
	return ret;
}

std::shared_ptr<Model> OBJLoader::load(const char * path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		return nullptr;
	}

	std::vector<Vec3f> positions;
	std::vector<Vec2f> uvs;
	std::vector<Vec3f> norms;

	std::map<std::string, std::shared_ptr<Material>> materials;

	std::shared_ptr<Mesh> mesh;
	auto model = std::make_shared<Model>();
	bool firstPos = true;
	while (!file.eof()) {
		std::string type;
		file>>type;
		if (type[0] == '#') {
			skipLine(file);
			continue;
		}
		if (type == "v") {
			Vec3f pos;
			file>>pos.x>>pos.y>>pos.z;
			positions.push_back(pos);
			auto & boundingBox = model->boundingBox;
			if (firstPos) {
				firstPos = false;
				boundingBox.min = pos;
				boundingBox.max = pos;
			}else {
				if (pos.x < boundingBox.min.x) {
					boundingBox.min.x = pos.x;
				}
				if (pos.y < boundingBox.min.y) {
					boundingBox.min.y = pos.y;
				}
				if (pos.z < boundingBox.min.z) {
					boundingBox.min.z = pos.z;
				}

				if (pos.x > boundingBox.max.x) {
					boundingBox.max.x = pos.x;
				}
				if (pos.y > boundingBox.max.y) {
					boundingBox.max.y = pos.y;
				}
				if (pos.z > boundingBox.max.z) {
					boundingBox.max.z = pos.z;
				}
			}
		}else if (type == "vt") {
			Vec2f uv;
			file>>uv.x>>uv.y;
			uvs.push_back(uv);
		}else if (type == "vn") {
			Vec3f norm;
			file>>norm.x>>norm.y>>norm.z;
			norms.push_back(norm);
		}else if (type == "f") {
			std::string line;
			//这里一堆分割可以优化
			for (int i = 0; i < 3; i ++) {
				std::string p;
				file>>p;
				Vertex v;
				auto idxs = split(p.c_str(), '/');
				v.position = positions[makeidx(stoi(idxs[0]), positions)];
				if (idxs.size() > 1) {
					v.uv = uvs[makeidx(stoi(idxs[1]), uvs)];
				}
				if (idxs.size() > 2) {
					v.norm = norms[makeidx(stoi(idxs[2]), norms)];
				}
				mesh->verties.push_back(v);
			}
		} else if (type == "o") {
			mesh = std::make_shared<Mesh>();
			file>>mesh->name;
			model->meshs.push_back(mesh);
		} else if (type == "usemtl") {
			std::string name;
			file>>name;
			if (mesh) {
				mesh->material = materials[name];
			}
		} else if (type == "mtllib") {
			std::string mtl;
			file>>mtl;
			//可能后面得改，可能不是相对路径
			materials = std::move(loadMtl((getParentPath(path) + mtl).c_str()));
		} else {
			skipLine(file);
		}
		fflush(stdout);

	}
	return model;
}
