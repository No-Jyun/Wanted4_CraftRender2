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
		void UpdateCameraMatrix(const Matrix4& viewMatrix);
		
		// DrawCall 발생 처리.
		// -> 렌더링 파이프라인 실행(구동).
		void DrawScene();

		static Renderer& Get();

	private:
		// 렌더 큐(Queue).
		std::vector<RenderCommand> renderQueue;

		// 카메라 행렬 버퍼
		ID3D11Buffer* cameraBuffer = nullptr;

		static Renderer* instance;
	};
}