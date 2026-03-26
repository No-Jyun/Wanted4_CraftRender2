#pragma once

#include "Math/Matrix4.h"
#include <vector>
#include <d3d11.h>
#include <memory>

namespace Craft
{
	class StaticMesh;
	class Shader;
	class Transform;

	// 렌더 명령(RenderCommand).
	struct RenderCommand
	{
		std::shared_ptr<StaticMesh> mesh;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Transform> transform;
	};

	// 카메라 버퍼 데이터
	struct CameraData
	{
		Matrix4 matrix;
		Vector3 position;
		// 상수 버퍼용 구조체는 16바이트 정렬이 필수 !!!
		float padding = 0.0f;
	};

	// 라이트 데이터 구조체
	struct LightData
	{
		// 위치 (조명의 방향 계산용)
		Vector3 position;
		// 강도 (세기)
		float intensity = 1.0f;
		// 색상
		Vector3 color = Vector3::One;
		// 패딩
		float padding = 0.0f;
	};

	// DrawCall 담당.
	// RHI - Render Hardware Interface: 그래픽카드.
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		// 초기화.
		void Initialize();

		// 그리는데 필요한 정보 제출.
		void Submit(
			std::shared_ptr<StaticMesh> mesh,
			std::shared_ptr<Shader> shader,
			std::shared_ptr<Transform> transform
		);

		// 카메라 행렬 제출 함수
		void UpdateCameraMatrix(
			const Matrix4& viewMatrix,
			const Matrix4& projectionMatrix,
			const Vector3& position);
		
		// 라이트 데이터 제출 함수
		void UpdateLightData(
			const Vector3& position,
			float intensity,
			const Vector3& color
		);

		// DrawCall 발생 처리.
		// -> 렌더링 파이프라인 실행(구동).
		void DrawScene();

		static Renderer& Get();

	private:
		// 렌더 큐(Queue).
		std::vector<RenderCommand> renderQueue;

		// 카메라 행렬 버퍼
		ID3D11Buffer* cameraBuffer = nullptr;

		// 라이트 버퍼
		ID3D11Buffer* lightBuffer = nullptr;

		static Renderer* instance;
	};
}