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

SkinnedMesh SkinnedModel::loadMesh(aiMesh * mesh, const aiScene * scene)
{
    SkinnedMesh ret;
    if (!mesh || !scene) {
        return ret;
    }

    for (int i = 0; i < mesh->mNumVertices; ++ i) {
        SkinnedVertex vertex;
        if (!mesh->mVertices) {
            continue;
        }
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        if (mesh->mNormals) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0]) {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }else {
            vertex.uv.x = 0;
            vertex.uv.y = 0;
        }
        ret.vertices.push_back(vertex);
    }

    for (int i = 0 ; i < mesh->mNumFaces; i ++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0 ; j < face.mNumIndices; j ++) {
            ret.indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        ret.material = std::make_shared<Material>();
		aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

        auto tryGetTexture = [=](decltype(aiTextureType_AMBIENT) type, std::shared_ptr<Texture> & out) {
            if (material->GetTextureCount(type) > 0) {
                aiString str;
                material->GetTexture(type, 0, &str);
                out = std::make_shared<Texture>((this->path + '/' + str.C_Str()).c_str());
            }
        };

        tryGetTexture(aiTextureType_AMBIENT, ret.material->ambientMap);
        tryGetTexture(aiTextureType_DIFFUSE, ret.material->diffuseMap);
        tryGetTexture(aiTextureType_SPECULAR, ret.material->specularMap);
        tryGetTexture(aiTextureType_HEIGHT, ret.material->normalMap);
    }

    return ret;
}

void SkinnedModel::processNode(aiNode * node, const aiScene * scene)
{
    for (int i = 0 ; i < node->mNumMeshes; i++) {
        aiMesh * m = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(loadMesh(m, scene));
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

bool SkinnedModel::load(const std::string & path)
{
    Assimp::Importer Importer;
    const aiScene* pScene = Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    this->path = std::string(path).substr(0, path.find_last_of('/'));
    processNode(pScene->mRootNode, pScene);
    assert(pScene && "resource not found");
    return true;
}
