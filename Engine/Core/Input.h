#pragma once

#include "Common.h"
#include "Math/Vector2.h"

namespace Craft
{
	// 키보드 입력 데이터
	struct KeyState
	{
		bool isKeyDown = false;
		bool isKeyUp = false;
		bool isKey = false;

		// 상태 설정 함수
		void SetKeyUpDown(bool isKeyUp, bool isKeyDown)
		{
			this->isKeyDown = isKeyDown;
			this->isKeyUp = isKeyUp;

			isKey = isKeyDown && !isKeyUp;
		}
	};

	// 마우스 입력 데이터
	struct MouseState
	{
		bool isButtonDown = false;
		bool isButtonUp = false;
		bool isButton = false;

		// 상태 설정 함수
		void SetButtonUpDown(bool isButtonUp, bool isButtonDown)
		{
			this->isButtonDown = isButtonDown;
			this->isButtonUp = isButtonUp;

			isButton = isButtonDown && !isButtonUp;
		}
	};

	class CRAFT_API Input
	{
	public:
		Input();
		~Input();

		// 싱글톤 접근 함수
		static Input& Get();

	private:

		// 키 / 버튼 상태 변수
		KeyState keyStates[256] = {};
		MouseState mouseStates[3] = {};

		// 마우스 위치 변수
		Vector2 mousePosition;
		Vector2 mousePreviousPosition;

		// 싱글톤 전역 변수
		static Input* instance;
	};
}
