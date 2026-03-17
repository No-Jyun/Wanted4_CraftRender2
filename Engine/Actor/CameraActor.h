#pragma once

#include "Actor.h"
#include "Math/Matrix4.h"

namespace Craft
{
	// 뷰 변환 행렬을 제공 및 처리하는 카메라 액터
	class CRAFT_API CameraActor : public Actor
	{
	public:
		CameraActor(
			float fieldOfView = 60.0f,
			float nearDistance = 0.1f,
			float farDistance = 1000.0f);
		~CameraActor();

		virtual void Tick(float deltaTime) override;
		virtual void Draw() override;

	private:
		// 뷰 변환 행렬
		// (RT)^(-1) = T^(-1) x R^(-1)
		Matrix4 viewMatrix;

		// 원근 투영 변환 행렬
		Matrix4 projectionMatrix;

		// 카메라 속성
		float fieldOfView = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float nearDistance = 0.0f;
		float farDistance = 0.0f;
	};
}
