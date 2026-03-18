#pragma once

#include "Core/Common.h"
#include "Matrix4.h"
#include <d3d11.h>

namespace Craft
{
	// 액터의 트랜스폼
	// 상용 엔진의 트랜스폼 컴포넌트가 하는 기능을 제공
	class CRAFT_API Transform
	{
	public:
		Transform();
		~Transform();

		// 프레임 마다 호출할 함수
		// T/R/S 업데이트 처리
		void Update();

		// 바인딩 ( CPU -> GPU )
		void Bind();

	public:
		// 트랜스폼 구성 성분
		Vector3 position = Vector3::Zero;
		Vector3 rotation = Vector3::Zero;
		Vector3 scale = Vector3::One;

		// 트랜스폼 기준 로컬 방향
		Vector3 Right() const;
		Vector3 Up() const;
		Vector3 Forward() const;

	private:
		// 트랜스폼을 구성하는 행렬
		Matrix4 transformMatrix;

		// 행렬을 Shader로 전달할 때 사용할 DX 리소스
		ID3D11Buffer* constantBuffer = nullptr;
	};
}
