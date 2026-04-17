#include "QuadMeshActor.h"
#include "Resource/MeshLoader.h"

#include "Shader/Shader.h"
#include "Shader/TextureMappingShader.h"
#include "Shader/SpecularMappingShader.h"
#include "Shader/NormalMappingShader.h"

#include "Texture/Texture.h"
#include "Texture/RenderTexture.h"
#include "Resource/TextureLoader.h"

#include "Math/Matrix4.h"
#include "Graphics/StaticMesh.h"
#include "Math/Transform.h"

using namespace Craft;

QuadMeshActor::QuadMeshActor()
{
	isUsingRenderTexture = true;

	// 메시 로드 요청
	MeshLoader::Get().GetMesh("Quad.obj", mesh);

	// 셰이더 생성
	//shader = std::make_shared<Shader>(L"Default");
	//std::shared_ptr<TextureMappingShader> textureMappingShader = std::make_shared<TextureMappingShader>("T_coord.png");
	//shader = std::make_shared<SpecularMappingShader>("2k_earth_daymap.jpg", "2k_earth_specular_map.jpg");

	// 셰이더 객체 생성
	std::shared_ptr<TextureMappingShader> shader = std::make_shared<TextureMappingShader>();

	// 렌더 텍스처 로드
	std::weak_ptr<RenderTexture> texture;
	TextureLoader::Get().GetNewRenderTexture(texture, 1280, 800);

	// 셰이더에 렌더 텍스처 설정
	shader->SetTexture(texture);

	// 액터 셰이더 목록에 추가	
	shaderList.emplace_back(shader);
}

void QuadMeshActor::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);

	// 스케일 테스트
	static float scale = 1.0f;

	static float direction = 1.0f;

	scale = scale + 0.5f * direction * deltaTime;
	if (scale >= 1.2f)
	{
		direction = -1.0f;
	}
	else if (scale <= 0.8f)
	{
		direction = 1.0f;
	}

	// 회전 테스트
	static float rotation = 0.0f;
	rotation += 60.0f * deltaTime;

	// 위치 테스트
	static float xPosition = 0.0f;
	xPosition += 1.0f * direction * deltaTime;

	// 적용 (점에다가)
	//auto vertices = mesh.lock()->GetVertices();
	//for (Vertex& vertex : vertices)
	//{
	//	vertex.position = vertex.position * Matrix4::Scale(scale);
	//	vertex.position = vertex.position * Matrix4::RotationZ(rotation);
	//}

	//// 정점 버퍼 업데이트
	//mesh.lock()->UpdateVertexBuffer(vertices);

	// 트랜스폼 변경
	//transform->rotation.z = rotation;
	//transform->scale = Vector3(scale, scale, scale);
	//transform->position.x = xPosition;
}
