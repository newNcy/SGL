#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#include "debug.h"

Texture::Texture(const char * path)
{
	data = stbi_load(path, &width, &height, &n, 0);
	//assert(data);
}

Vec4f Texture::sample(float u, float v)
{
    if (!data) {
        return Vec4f(1.f, 1.f, 1.f, 1.f);
    }
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

Vec3f readVec3f(const aiVector3D & vec3)
{
    return Vec3f(vec3.x, vec3.y, vec3.z);
}

Quat readQuat(const aiQuaternion & quat)
{
    return {quat.x, quat.y, quat.z, quat.w};
}

Mat4f readMat4f(const aiMatrix4x4t<float> & matrix)
{
    Mat4f ret;
    for (int j = 0; j < 4; ++ j) {
        for (int k = 0; k < 4; ++ k) {
            ret[j][k] = matrix[j][k];
        }
    }
    return ret;
}

SkinnedMesh SkinnedModel::loadMesh(aiMesh * mesh, const aiScene * scene)
{
    SkinnedMesh ret;
    if (!mesh || !scene) {
        return ret;
    }

    uint32_t baseIdx = 0;
    for (auto & m : meshes){
        baseIdx += m.vertices.size();
    }

    bool firstPos = true;
    for (int i = 0; i < mesh->mNumVertices; ++ i) {
        SkinnedVertex vertex;
        if (!mesh->mVertices) {
            continue;
        }
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        if (firstPos) {
            firstPos = false;
            boundingBox.min = vertex.position;
            boundingBox.max = vertex.position;
        }else {
            if (vertex.position.x < boundingBox.min.x) {
                boundingBox.min.x = vertex.position.x - 0.02;
            }
            if (vertex.position.y < boundingBox.min.y) {
                boundingBox.min.y = vertex.position.y - 0.02;
            }
            if (vertex.position.z < boundingBox.min.z) {
                boundingBox.min.z = vertex.position.z - 0.02;
            }

            if (vertex.position.x > boundingBox.max.x) {
                boundingBox.max.x = vertex.position.x + 0.02;
            }
            if (vertex.position.y > boundingBox.max.y) {
                boundingBox.max.y = vertex.position.y + 0.02;
            }
            if (vertex.position.z > boundingBox.max.z) {
                boundingBox.max.z = vertex.position.z + 0.02;
            }
        }
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

    //load bones
    for (int i = 0 ; i < mesh->mNumBones; ++ i) {
        auto bone = mesh->mBones[i];
        auto iter = boneIDMap.find(bone->mName.data);
        uint32_t boneID;
        if (iter == boneIDMap.end()) {
            Bone b;
            b.name = bone->mName.data;

            b.offset = readMat4f(bone->mOffsetMatrix);
            boneID = boneIDMap[b.name] = bones.size();
            bones.push_back(b);
        }else{
            boneID = iter->second;
        }

        for (int i = 0 ; i < bone->mNumWeights; ++i) {
            auto & weight = bone->mWeights[i];
            ret.vertices[weight.mVertexId].bindToBone(boneID, weight.mWeight);
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
    assert(pScene && "resource not found");
    processNode(pScene->mRootNode, pScene);
    return true;
}


void Animation::getNodeTransform(double sec, std::shared_ptr<SkeletonNode> node, const Mat4f & inverse, const std::unordered_map<std::string, uint32_t> & boneIDMap, const std::vector<Bone> & bones, Frame & frame)
{
    if (!node) return;

    auto iter = animationChannels.find(node->name);
    node->transform = Mat4f::identify;
    if (iter != animationChannels.end()) {
        auto & c = iter->second;
        Mat4f translation = Mat4f::identify;
        Mat4f scaling = Mat4f::identify;
        Mat4f rotation = Mat4f::identify;
        for (int i = 0; i < c.positionKeys.size()-1; ++i) {
            auto & a = c.positionKeys[i];
            auto & b = c.positionKeys[i+1];
            if (a.time <= sec && sec < b.time) {
                double t = (sec - a.time) / (b.time - a.time);
                auto value = lerp(a.value, b.value, t);
                translation[3][0] = value.x;
                translation[3][1] = value.y;
                translation[3][2] = value.z;
            }
        }
        for (int i = 0; i < c.scalingKeys.size()-1; ++i) {
            auto & a = c.scalingKeys[i];
            auto & b = c.scalingKeys[i+1];
            if (a.time <= sec && sec < b.time) {
                double t = (sec - a.time) / (b.time - a.time);
                auto value = lerp(a.value, b.value, t);
                scaling[0][0] = value.x;
                scaling[1][1] = value.y;
                scaling[2][2] = value.z;
            }
        }
        for (int i = 0; i < c.rotationKeys.size()-1; ++i) {
            auto & a = c.rotationKeys[i];
            auto & b = c.rotationKeys[i+1];
            if (a.time <= sec && sec < b.time) {
                double t = (sec - a.time) / (b.time - a.time);
                auto rotator = lerp(a.value, b.value, t);
                rotation = rotator/rotator.length();
            }
        }
        node->transform = scaling * rotation *translation ; 
    }

    if (node->parent) {
        node->transform = node->transform * node->parent->transform;
    }

    auto biter = boneIDMap.find(node->name);
    if (biter != boneIDMap.end()) {
        auto index = biter->second;
        frame.jointPoses[index] = bones[index].offset * node->transform;
    }

    for (auto child : node->childs) {
        getNodeTransform(sec, child, inverse, boneIDMap, bones, frame);
    }
}

Frame Animation::getFrame(double sec, Skeleton & sk, const std::unordered_map<std::string, uint32_t> & boneIdMap, const std::vector<Bone> & bones)
{
    Frame frame;
    sec = sec * ticksPerSecond;
    sec = fmod(sec, duration);
    frame.jointPoses.resize(bones.size());
    getNodeTransform(sec, sk.root, sk.globalInverse, boneIdMap, bones, frame);
    return frame;
}

std::shared_ptr<SkeletonNode> AnimationSet::processNode(aiNode * node, const aiScene * scene)
{
    auto currentNode = std::make_shared<SkeletonNode>();
    currentNode->name = node->mName.C_Str();
    currentNode->transform = readMat4f(node->mTransformation);
    for (int i = 0; i < node->mNumChildren; i++) {
        auto child = processNode(node->mChildren[i], scene);
        child->parent = currentNode;
        currentNode->childs.push_back(child);
    }
    return currentNode;
}

bool AnimationSet::load(const std::string & path)
{
    Assimp::Importer Importer;
    const aiScene* pScene = Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    assert(pScene);
	if (pScene && pScene->mRootNode && pScene->mRootNode->mNumChildren) {
		skeleton.root = processNode(pScene->mRootNode, pScene);
		skeleton.globalInverse = readMat4f(pScene->mRootNode->mTransformation.Inverse()); 
	}
    for (int i = 0 ; i < pScene->mNumAnimations; ++i) {
        auto anim = pScene->mAnimations[i];
        auto & animation = animations[anim->mName.C_Str()];
        animation.duration = anim->mDuration;
        animation.ticksPerSecond = anim->mTicksPerSecond;
        for (int j = 0 ; j < anim->mNumChannels; ++j) {
            auto channel = anim->mChannels[j];
            //printf("animation channel node name [%s]\n", channel->mNodeName.C_Str());
            auto & animationChannel = animation.animationChannels[channel->mNodeName.C_Str()];

            for (int k = 0; k < channel->mNumPositionKeys; ++k) {
                auto posKey = channel->mPositionKeys[k];
                auto pos = posKey.mValue;

                AnimationKey<Vec3f> positionKey;
                positionKey.time = posKey.mTime;
                positionKey.value = readVec3f(pos);
                animationChannel.positionKeys.push_back(positionKey);
            }

            for (int k = 0; k < channel->mNumScalingKeys; ++k) {
                auto key = channel->mScalingKeys[k];
                auto scale = key.mValue;

                AnimationKey<Vec3f> scalingKey;
                scalingKey.time = key.mTime;
                scalingKey.value = readVec3f(scale);
                animationChannel.scalingKeys.push_back(scalingKey);
            }
            
            for (int k = 0; k < channel->mNumRotationKeys; ++k) {
                auto key = channel->mRotationKeys[k];
                auto rotation = key.mValue;

                AnimationKey<Quat> rotationKey;
                rotationKey.time = key.mTime;
                rotationKey.value = readQuat(rotation);
                animationChannel.rotationKeys.push_back(rotationKey);
            }
        }
    }

    return true;
}
