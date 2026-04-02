#include "CameraActor.h"
#include "Math/Transform.h"
#include "Graphics/Renderer.h"
#include "Core/Engine.h"

namespace Craft
{
	CameraActor::CameraActor(
		float fieldOfView,
		float nearDistance,
		float farDistance)
		: fieldOfView(fieldOfView), 
		width(static_cast<float>(Engine::Get().GetWidth())),
		height(static_cast<float>(Engine::Get().GetHeight())),
		nearDistance(nearDistance),
		farDistance(farDistance)
	{
		// 뷰 행렬 생성
		// 위치 변환 행렬의 역변환
		Matrix4 translation = Matrix4::Translation(transform->position * -1.0f);

		// 위치 변환 행렬 
		Matrix4 rotation = Matrix4::Rotation(transform->rotation);

		// 카메라 행렬 업데이트
		viewMatrix = translation * Matrix4::Inverse(rotation);

		// 투영 행렬 생성
		projectionMatrix = Matrix4::Perspective(
			fieldOfView,
			width,
			height,
			nearDistance,
			farDistance
		);
	}

	CameraActor::~CameraActor()
	{
	}

	void CameraActor::OnResize(uint32_t width, uint32_t height)
	{
		// 투영 변환 행렬 업데이트
		projectionMatrix = Matrix4::Perspective(
			fieldOfView,
			static_cast<float>(width),
			static_cast<float>(height),
			nearDistance,
			farDistance
		);
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

		// 투영 행렬 업데이트 (해도 되고 안해도 됨 / 우리는 거의 변하지 않으므로)
		//perspective = Matrix4::Perspective(
		//	fieldOfView,
		//	width,
		//	height,
		//	nearDistance,
		//	farDistance
		//);
	}

	void CameraActor::Draw()
	{
		Actor::Draw();

		// 렌더러에 행렬 제출
		Renderer::Get().UpdateCameraMatrix(viewMatrix, projectionMatrix, transform->position);
	}
}