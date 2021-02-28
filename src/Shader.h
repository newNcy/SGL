#pragma once 
#include "maths.h"
#include <memory>
#include "debug.h"
#include "Model.h"
#include "global.h"

struct V2f 
{
	Vec4f position; 
	Vec3f color;	//颜色
	Vec2f uv;		//纹理坐标
	Vec3f norm;		//法向量
	Vec3f worldPosition; //世界坐标
};

class SGLShader
{	
	public:
		virtual void onVertex(const Vertex & vert, V2f & out) 
		{ 
		}
		virtual void onFragment(const V2f & v2f, Vec4f & color) 
		{ color = Vec4f(v2f.color,1); }
		~SGLShader() {}
};

class NormalShader : public SGLShader
{
	
	protected:
		const float pi = 3.1415926;
		Mat4f model, view, projection, normalMatrix;
	public:
		void setModel(const Mat4f & m) 
		{
			model = m;
            normalMatrix = m;
            normalMatrix[3] = Vec4f(0, 0, 0, 1);
		}

        void setNormalMatrix(const Mat4f & nm)
        {
            normalMatrix = nm;
        }

		void setCamera(const Mat4f & c)
		{
			view = c;
		}

		NormalShader()
		{
			//print(view);
			projection = perspective(60, 1.f, .1f, 1000.f);
		}

		void onVertex(const Vertex & vert, V2f & out) override
		{
			PROFILE(vertex_shader);
			out.position = Vec4f(vert.position,1) * model * view * projection;
			auto worldPos = Vec4f(vert.position, 1) * model;
			auto worldNorm = Vec4f(vert.norm, 1) * normalMatrix;
			out.worldPosition = Vec3f(worldPos.x, worldPos.y, worldPos.z);
			out.norm = Vec3f(worldNorm.x, worldNorm.y, worldNorm.z);
		}
};

class TextureShader : public NormalShader
{
	protected:
		std::shared_ptr<Texture> texture;
	public:
		TextureShader()
		{
			texture.reset(new Texture("../resource/image/container.jpg"));
		}

		void onFragment(const V2f & v, Vec4f & color) override
		{
			PROFILE(fragment_shader);
			color = v.norm; //texture->sample(v.uv.u, v.uv.v);
		}
};

/*
 * 通常使用shader
 * 使用材质和冯氏光照模型
 */
struct ParallelLight
{
	Vec3f direct;
	Vec3f color;
};

struct PointLight
{
	Vec3f position;
	Vec3f color;
};

class ModelShader : public NormalShader
{
	protected:
		std::shared_ptr<Material> material;
	public:
		std::map<std::string, ParallelLight> parallelLights;
		std::map<std::string, PointLight> pointLights;

		void setMaterial(const std::shared_ptr<Material> & material) 
		{
			this->material = material;
		}
		void onFragment(const V2f & v, Vec4f & color) override;

};
