#include "TriangleMesh.h"
#include "Graphics/Vertex.h"

using namespace Craft;

TriangleMesh::TriangleMesh()
{
	// 정점 데이터 (3개의 점).
	Vertex vertices[] =
	{
		Vertex(Vector3(0.0f, 0.5f, 0.1f)),		// 첫 번째 점의 위치(3차원 좌표).
		Vertex(Vector3(0.5f, -0.5f, 0.1f)),		// 두 번째 점의 위치(3차원 좌표).
		Vertex(Vector3(-0.5f, -0.5f, 0.1f)),	// 세 번째 점의 위치(3차원 좌표).
	};

	// 인덱스 (삼각형 조합할 때 사용할 순서를 지정).
	uint32_t indices[] = { 0, 1, 2 };

	StaticMesh::Initialize(
		vertices, _countof(vertices), Vertex::Stride(), indices, _countof(indices)
	);
}