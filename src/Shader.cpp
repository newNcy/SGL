#include "Shader.h"


void ModelShader::onFragment(const V2f & v, Vec4f & color)
{
	PROFILE(fragment_shader);
	float ambientFactor = 0.1f;
	//环境光
	Vec4f ambientColor;
	if (material->ambientMap) {
		ambientColor = material->ambientMap->sample(v.uv.u, v.uv.v);
	}
	//散射
	Vec3f diffuseColor;
	for (auto & dlight : parallelLights) {
		auto dir = normalize(dlight.second.direct*-1);
		auto norm = normalize(v.norm);
		float diff = std::max(dot(dir, norm), 0.f);
		diffuseColor = diffuseColor + dlight.second.color * diff;
	}

	color = ambientColor + material->diffuseMap->sample(v.uv.u, v.uv.v) * diffuseColor;
	color.a = 1;
}
