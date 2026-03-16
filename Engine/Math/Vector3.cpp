#include "Vector3.h"
#include <cmath>
#include <cassert>

namespace Craft 
{
	Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
	Vector3 Vector3::One(1.0f, 1.0f, 1.0f);
	Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
	Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
	Vector3 Vector3::Forward(0.0f, 0.0f, 1.0f);
	
	Vector3::Vector3(float x, float y, float z)
		: x(x), y(y), z(z)
	{
	}

	Vector3 Vector3::operator+(const Vector3& other) const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 Vector3::operator-(const Vector3& other) const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}
	
	Vector3 Vector3::operator*(const Vector3& other) const
	{
		return Vector3(x * other.x, y * other.y, z * other.z);
	}
	
	Vector3 Vector3::operator*(float scale) const
	{
		return Vector3(x * scale, y * scale, z * scale);
	}

	float Dot(const Vector3& left, const Vector3& right)
	{
		return left.x * right.x + left.y * right.y + left.z * right.z;
	}

	Vector3 Cross(const Vector3& left, const Vector3& right)
	{
		// 3중적 (두 벡터와 직교하는 다른 벡터를 구하는 벡터의 곱셈)
		// !! 곱셈의 순서가 중요. 교환법칙 성립 안함
		// A X B = A X B X Sin(Theta)
		return Vector3(
			(left.y * right.z) - (left.z * right.y),
			(left.z * right.x) - (left.x * right.z),
			(left.x * right.y) - (left.y * right.x)
		);
	}

	// 벡터 길이 구하는 함수
	float Vector3::Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	// 정규화 함수 (벡터의 크기를 1로 만드는 함수)
	Vector3 Vector3::Normalized() const
	{
		// 모든 컴포넌트 (항목) 을 벡터의 길이 값으로 나누기
		float length = Length();

		// 0으로 나누기 방지
		assert(length > 0);

		// 정규화 벡터 반환
		return Vector3(x / length, y / length, z / length);
	}
}