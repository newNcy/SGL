#include "debug.h"
#include "Pipeline.h"
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
