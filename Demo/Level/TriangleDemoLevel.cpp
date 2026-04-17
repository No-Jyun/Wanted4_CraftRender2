#include "TriangleDemoLevel.h"
#include "Actor/TriangleMeshActor.h"
#include "Actor/QuadMeshActor.h"
#include "Actor/CameraMovementActor.h"

#include "Actor/SkyboxActor.h"

#include "Actor/WarriorCharacter.h"

#include "Actor/LightActor.h"

#include "Math/Transform.h"

using namespace Craft;

void TriangleDemoLevel::OnInitialized()
{
	Level::OnInitialized();

	//SpawnActor<TriangleMeshActor>();
	//std::shared_ptr<QuadMeshActor> quad = SpawnActor<QuadMeshActor>();
	//quad->transform->position.x = 0.5f;
	
	//std::shared_ptr<QuadMeshActor> quad2 = SpawnActor<QuadMeshActor>();
	//quad2->transform->position.x = -0.5f;

	std::shared_ptr<WarriorCharacter> warrior = SpawnActor<WarriorCharacter>();
	warrior->transform->rotation.x = 90.0f;


	// 카메라 액터 생성
	//std::shared_ptr<CameraActor> camera = SpawnActor<CameraActor>();
	std::shared_ptr<CameraMovementActor> camera = SpawnActor<CameraMovementActor>();
	//camera->transform->position.x = 0.5f;
	camera->transform->position.z = -250.0f;
	camera->transform->position.y = 90.0f;
	//camera->transform->rotation.y = -2.0f;
	camera->SetMoveSpeed(100.0f);

	// 라이트 액터 생성
	std::shared_ptr<LightActor> light = SpawnActor<LightActor>();
	light->transform->position = Vector3(-500.0f, 500.0f, -500.0f);
	light->SetColor(Vector3(1.0f, 1.0f, 0.0f));

	// 스카이 박스 추가
	std::shared_ptr<SkyboxActor> skybox = SpawnActor<SkyboxActor>();

	// 스케일 엄청 키우기
	skybox->transform->scale = Vector3::One * 10000.0f;

	// 쿼드 메시 액터 추가
	std::shared_ptr<QuadMeshActor> quad = SpawnActor<QuadMeshActor>();
	
	quad->transform->position.x = 100.0f;
	quad->transform->position.y = 100.0f;
	quad->transform->scale = Vector3::One * 100.0f;
}