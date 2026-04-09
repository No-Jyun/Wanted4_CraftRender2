#include "TriangleMeshActor.h"
#include "Shader/Shader.h"
#include "Mesh/TriangleMesh.h"

TriangleMeshActor::TriangleMeshActor()
{
	mesh = std::make_shared<TriangleMesh>();
	shaderList.emplace_back(std::make_shared<Craft::Shader>(L"Default"));
}