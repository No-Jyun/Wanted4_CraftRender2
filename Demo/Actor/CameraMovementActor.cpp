#include "CameraMovementActor.h"
#include "Core/Input.h"
#include "Math/Transform.h"

using namespace Craft;

CameraMovementActor::CameraMovementActor()
{
}

void CameraMovementActor::Tick(float deltaTime)
{
	CameraActor::Tick(deltaTime);

	// ESC 키 종료
	if (Input::Get().IsKeyDown(VK_ESCAPE))
	{
		QuitGame();
	}

	// 좌우 이동
	if (Input::Get().IsKey(VK_RIGHT) || Input::Get().IsKey('D'))
	{
		transform->position = transform->position + transform->Right() * deltaTime * moveSpeed;
	}

	if (Input::Get().IsKey(VK_LEFT) || Input::Get().IsKey('A'))
	{
		transform->position = transform->position - transform->Right() * deltaTime * moveSpeed;
	}

	if (Input::Get().IsKey(VK_UP) || Input::Get().IsKey('W'))
	{
		transform->position = transform->position + transform->Forward() * deltaTime * moveSpeed;
	}

	if (Input::Get().IsKey(VK_DOWN) || Input::Get().IsKey('S'))
	{
		transform->position = transform->position - transform->Forward() * deltaTime * moveSpeed;
	}

	if (Input::Get().IsKey('E'))
	{
		transform->position
			= transform->position + transform->Up() * deltaTime * moveSpeed;
	}

	if (Input::Get().IsKey('Q'))
	{
		transform->position
			= transform->position - transform->Up() * deltaTime * moveSpeed;
	}

	if (Input::Get().IsButton(0))
	{
		// 좌우 드래그 처리
		transform->rotation.y += dragSpeedOffset * Input::Get().GetMouseDeltaX();
		transform->rotation.x += dragSpeedOffset * Input::Get().GetMouseDeltaY();
	}
}
