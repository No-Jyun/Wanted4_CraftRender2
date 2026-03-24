#pragma once

#include "Core/Common.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include <stdint.h>

namespace Craft
{
	// 메시 / 정점 셰이더에서 사용할 정점 클래스
	// 그래픽스 파이프라인의 입력인 '점'의 구조를 정의
	class CRAFT_API Vertex
	{
	public:
		// 정점을 넘길 때 레이아웃 중요
		// 위치 (Position)
		Vector3 position;

		// 텍스처 좌표 (Texture Coordinates, UV 좌표)
		Vector2 texCoord;

		// 노멀 (법선) 벡터
		Vector3 normal;

		Vertex(const Vector3& position, const Vector2 texCoord = Vector2::Zero, const Vector3& normal = Vector3::Zero);
		~Vertex() = default;

		// 메모리 너비 (Stride)
		// sizeof(position) + sizeof(uv) + ...
		static uint32_t Stride() { return sizeof(Vertex); }
	};
}
