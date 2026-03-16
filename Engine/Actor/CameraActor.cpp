#include "CameraActor.h"
#include "Math/Transform.h"
#include "Graphics/Renderer.h"

namespace Craft
{
	CameraActor::CameraActor()
	{
	}

	CameraActor::~CameraActor()
	{
	}

	void CameraActor::Tick(float deltaTime)
	{
		Actor::Tick(deltaTime);

		// 위치 변환 행렬의 역변환
		Matrix4 translation = Matrix4::Translation(transform->position * -1.0f);
		
		// 위치 변환 행렬 
		Matrix4 rotation = Matrix4::Rotation(transform->rotation);

		// 카메라 행렬 업데이트
		viewMatrix = translation * Matrix4::Inverse(rotation);
	}

	void CameraActor::Draw()
	{
		Actor::Draw();

		// 렌더러에 행렬 제출
		Renderer::Get().UpdateCameraMatrix(viewMatrix);
	}
}