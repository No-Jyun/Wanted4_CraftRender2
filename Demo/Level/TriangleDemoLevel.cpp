#include "TriangleDemoLevel.h"
#include "Actor/TriangleMeshActor.h"
#include "Actor/QuadMeshActor.h"
#include "Actor/CameraMovementActor.h"

#include "Actor/WarriorCharacter.h"
#include "Actor/Freddy.h"

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

	//std::shared_ptr<WarriorCharacter> warrior = SpawnActor<WarriorCharacter>();
	//warrior->transform->rotation.x = 90.0f;

	std::shared_ptr<Freddy> freddy = SpawnActor<Freddy>();

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
}