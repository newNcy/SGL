#pragma once
#include <vector>
#include <map>
#include <memory>
#include "Shader.h"
#include "Model.h"

/*
 * 模型加载器
 */
class ModelLoader
{
	public:
		virtual std::shared_ptr<Model> load(const char * path) = 0;
};

/*
 * obj文件加载器
 */
class OBJLoader : public ModelLoader
{

	protected:
		std::map<std::string, std::shared_ptr<Material>> loadMtl(const char * path);
	public:
		std::shared_ptr<Model> load(const char * path) override;
};
