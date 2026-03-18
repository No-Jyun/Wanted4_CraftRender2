#include "Input.h"

namespace Craft
{
	// static 변수 정의.
	Input* Input::instance = nullptr;

	Input::Input()
	{
		// 싱글톤 변수 설정.
		instance = this;
	}

	Input::~Input()
	{
	}

	bool Input::IsKeyDown(unsigned int keyCode)
	{
		return keyStates[keyCode].isKeyDown;
	}

	bool Input::IsKeyUp(unsigned int keyCode)
	{
		return keyStates[keyCode].isKeyUp;
	}

	bool Input::IsKey(unsigned int keyCode)
	{
		return keyStates[keyCode].isKey;
	}

	bool Input::IsButtonDown(unsigned int button)
	{
		return mouseStates[button].isButtonDown;
	}

	bool Input::IsButtonUp(unsigned int button)
	{
		return mouseStates[button].isButtonUp;
	}

	bool Input::IsButton(unsigned int button)
	{
		return mouseStates[button].isButton;
	}

	void Input::ResetInputs()
	{
		for (KeyState& data : keyStates)
		{
			data.isKeyUp = false;
			data.isKeyDown = false;
		}

		for (MouseState& data : mouseStates)
		{
			data.isButtonDown = false;
			data.isButtonUp = false;
		}

		mousePreviousPosition = mousePosition;
	}

	Vector2 Input::GetMousePosition()
	{
		return mousePosition;
	}

	float Input::GetMouseDeltaX()
	{
		return mousePosition.x - mousePreviousPosition.x;
	}

	float Input::GetMouseDeltaY()
	{
		return mousePosition.y - mousePreviousPosition.y;
	}

	void Input::SetKeyUpDown(unsigned int keyCode, bool isKeyUp, bool isKeyDown)
	{
		keyStates[keyCode].SetKeyUpDown(isKeyUp, isKeyDown);
	}

	void Input::SetButtonUpDown(unsigned int button, bool isButtonUp, bool isButtonDown)
	{
		mouseStates[button].SetButtonUpDown(isButtonUp, isButtonDown);
	}

	void Input::SetMousePosition(int x, int y)
	{
		mousePreviousPosition = mousePosition;
		mousePosition = Vector2((float)x, (float)y);
	}

	Input& Input::Get()
	{
		return *instance;
	}

	bool Input::IsValid()
	{
		return instance != nullptr;
	}
}