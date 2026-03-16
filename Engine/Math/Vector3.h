#pragma once

#include "Core/Common.h"

namespace Craft
{
	class CRAFT_API Vector3
	{
	public:
		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		~Vector3() = default;

		// 벡터의 컴포넌트
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		// 연산자 오버로딩
		Vector3 operator+(const Vector3& other) const;
		Vector3 operator-(const Vector3& other) const;
		Vector3 operator*(const Vector3& other) const;
		Vector3 operator*(float scale) const;

		// 내적 (Dot Product / Inner Product)
		// A Dot B = A(크기) * B(크기) * Cos(Theta)
		friend CRAFT_API float Dot(const Vector3& left, const Vector3& right);

		// 외적 (Cross Product / Outer Product)
		friend CRAFT_API Vector3 Cross(const Vector3& left, const Vector3& right);

		// 벡터 길이 구하는 함수
		float Length() const;

		// 정규화 함수 (벡터의 크기를 1로 만드는 함수)
		Vector3 Normalized() const;

		// 전역 변수
		static Vector3 Zero;
		static Vector3 One;
		static Vector3 Right;
		static Vector3 Up;
		static Vector3 Forward;
	};
}
