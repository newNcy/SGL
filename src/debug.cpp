#include "debug.h"
#include "Pipeline.h"
#include <queue>
int Profiler::tab = 0;
Profiler * Profiler::firstNode= nullptr;
std::vector<ProfileEntry> Profiler::hits;

void print(const Mat4f & m, char end)
{
	printf("called\n");
	printf("{\n");
	for (int i = 0 ; i < 4; ++ i) {
		printf("\t");
		print(m[i], 4);
	}
	printf("}%c", end);
	fflush(stdout);
}
        
DebugRenderer::DebugRenderer()
{
}
void DebugRenderer::drawBoundingBox(SGLPipeline & pipeline, const BoundingBox3d & box, std::shared_ptr<SGLShader> shader)
{
    Vec3f color = {1.f, 1.f, 1.f};
    std::vector<Vertex> edges = {
        //bottom
        {{box.min.x, box.min.y, box.min.z},color},{{box.min.x, box.min.y, box.max.z},color},
        {{box.min.x, box.min.y, box.min.z},color},{{box.max.x, box.min.y, box.min.z},color},
        {{box.max.x, box.min.y, box.min.z},color},{{box.max.x, box.min.y, box.max.z},color},
        {{box.min.x, box.min.y, box.max.z},color},{{box.max.x, box.min.y, box.max.z},color},

        //along side
        {{box.min.x, box.min.y, box.min.z},color},{{box.min.x, box.max.y, box.min.z},color},
        {{box.min.x, box.min.y, box.max.z},color},{{box.min.x, box.max.y, box.max.z},color},
        {{box.max.x, box.min.y, box.max.z},color},{{box.max.x, box.max.y, box.max.z},color},
        {{box.max.x, box.min.y, box.min.z},color},{{box.max.x, box.max.y, box.min.z},color},

        //top
        {{box.min.x, box.max.y, box.min.z},color},{{box.min.x, box.max.y, box.max.z},color},
        {{box.min.x, box.max.y, box.min.z},color},{{box.max.x, box.max.y, box.min.z},color},
        {{box.max.x, box.max.y, box.min.z},color},{{box.max.x, box.max.y, box.max.z},color},
        {{box.min.x, box.max.y, box.max.z},color},{{box.max.x, box.max.y, box.max.z},color},
    };

    pipeline.useShader(shader);
    pipeline.drawArray(&edges[0], edges.size(), DrawMode::LINE);
}

    Vec3f color = {1,1,1};
void travelNode(std::shared_ptr<SkeletonNode> root, Mat4f parentTrans, std::vector<Vertex> & vs)
{
    auto trans = root->transform * parentTrans;
    auto pos = Vec4f(0,0,0,1) * trans;
    Vertex v = {{pos.x, pos.y, pos.z}, color};
    printf("%s (%.2f, %.2f, %.2f)\n", root->name.c_str(), pos.x, pos.y, pos.z);
    vs.push_back(v);

    for (auto child : root->childs) {
        travelNode(child, trans, vs);
    }
}


void DebugRenderer::drawSkeleton(SGLPipeline & pipeline, const Skeleton & sk, std::shared_ptr<NormalShader> shader)
{
    pipeline.useShader(shader);
    std::vector<Vertex> vs;
    travelNode(sk.root, Mat4f(), vs);
    pipeline.drawArray(vs.data(), vs.size(), DrawMode::LINE);
}
