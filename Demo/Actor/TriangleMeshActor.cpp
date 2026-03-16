#include "TriangleMeshActor.h"
#include "Shader/Shader.h"
#include "Mesh/TriangleMesh.h"

TriangleMeshActor::TriangleMeshActor()
{
	mesh = std::make_shared<TriangleMesh>();
	shader = std::make_shared<Craft::Shader>(L"Default");
}