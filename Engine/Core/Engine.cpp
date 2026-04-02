#include "Engine.h"
#include "Win32Window.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/Renderer.h"
#include "Level/Level.h"
#include "Resource/MeshLoader.h"
#include "Resource/TextureLoader.h"
#include "Core/Input.h"
#include <windowsx.h>

namespace Craft
{
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		instance = this;
	}

	Engine::~Engine()
	{
	}

	bool Engine::Initialize(HINSTANCE instance)
	{
		// 창 객체 생성.
		window = std::make_unique<Win32Window>(
			setting.width,
			setting.height,
			setting.title,
			instance,
			Win32MessageProcedure
		);

		// 창 초기화.
		if (!window->Initialize())
		{
			__debugbreak();
			return false;
		}

		// 컨텍스트 객체 생성.
		graphicsContext = std::make_unique<GraphicsContext>();

		// 초기화.
		graphicsContext->Initialize(*window);

		// 렌더러 생성.
		renderer = std::make_unique<Renderer>();
		renderer->Initialize();

		// 메시 로더 객체 생성
		meshLoader = std::make_unique<MeshLoader>();

		// 텍스처 로더 객체 생성
		textureLoader = std::make_unique<TextureLoader>();

		// 입력 객체 생성
		input = std::make_unique<Input>();

		return true;
	}

	void Engine::Run()
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		int64_t currentTime = counter.QuadPart;
		int64_t previousTime = currentTime;

		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		float oneFrameTime = 1.0f / setting.framerate;

		// 메시지 처리 루프.
		// 창에서 발생하는 메시지 처리 루프.
		// GetMessage - 동기 방식(Blocking 방식).
		// PeekMessage - 비동기 방식(Non Blocking 방식).
		// 동기: 처리되기 전까지 다른 동작 안함.
		// 비동기: 처리되지 않아도 넘어감.
		MSG msg = { };

		// 창 종료 메시지가 발생할 때까지 실행.
		while (msg.message != WM_QUIT)
		{
			// 엔진 종료 처리
			if (isQuit)
			{
				break;
			}

			// 창 메시지 처리.
			// 비동기 방식으로 메시지가 들어왔는지 확인.
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				// 전달받은 메시지를 적절하게 변환.
				TranslateMessage(&msg);

				// 변환한 메시지를 처리 함수에 전달.
				DispatchMessage(&msg);
			}

			// 창 메시지가 없으면 엔진 로직 처리.
			else
			{
				QueryPerformanceCounter(&counter);
				currentTime = counter.QuadPart;

				float deltaTime = static_cast<float>(currentTime - previousTime)
					/ static_cast<float>(frequency.QuadPart);

				if (deltaTime >= oneFrameTime)
				{
					OnInitialized();

					BeginPlay();

					Tick(deltaTime);

					Draw();

					if (mainLevel)
					{
						mainLevel->ProcessAddAndDestroyActors();
					}

					if (nextLevel)
					{
						if (mainLevel)
						{
							mainLevel.reset();
						}

						mainLevel = std::move(nextLevel);
						nextLevel.reset();
					}

					// 입력 정리
					input->ResetInputs();

					previousTime = currentTime;
				}
			}
		}
	}

	void Engine::Quit()
	{
		isQuit = true;
	}

	void Engine::OnResize(uint32_t width, uint32_t height)
	{
		// 그래픽스 컨텍스트에게 넘겨주기
		if (graphicsContext)
		{
			graphicsContext->OnResize(width, height);
		}

		// 창 객체의 변수 값 조정
		window->SetWidthAndHeight(width, height);

		// 카메라에 전달하기 : 레벨 -> 카메라 액터에 전달
		if (mainLevel)
		{
			mainLevel->OnResize(width, height);
		}
	}

	Engine& Engine::Get()
	{
		assert(instance);
		return *instance;
	}

	// 화면 너비 / 높이 반환 함수
	uint32_t Engine::GetWidth() const
	{
		return window->Width();
	}

	uint32_t Engine::GetHeight() const
	{
		return window->Height();
	}

	LRESULT Engine::Win32MessageProcedure(
		HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(handle, &ps);

			// All painting occurs here, between BeginPaint and EndPaint.
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(handle, &ps);
		}
		return 0;

		// 창 크기 변경 이벤트 처리
		case WM_SIZE:
		{
			// 최소화인 경우 무시
			if (wparam == SIZE_MINIMIZED)
			{
				break;
			}

			// null 체크
			if (!instance)
			{
				break;
			}

			// 변경된 창 크기 값 가져오기
			uint32_t width = LOWORD(lparam);
			uint32_t height = HIWORD(lparam);

			// 변경 이벤트 발행
			instance->OnResize(width, height);

		}
		return 0;

		// 키 입력 처리.
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			// 포커스 확인.
			if (GetFocus() != handle)
			{
				break;
			}

			// 예외 처리.
			if (!Input::IsValid())
			{
				break;
			}

			// 키 눌림/해제.
			bool isKeyUp = (lparam & ((int64_t)1 << 30)) != 0;
			bool isKeyDown = (lparam & ((int64_t)1 << 31)) == 0;

			// 두 상태가 다를 때만 이벤트 전달.
			if (isKeyUp != isKeyDown)
			{
				// 어떤 키가 눌렸는지 확인 -> wparam.
				uint32_t vkCode = static_cast<uint32_t>(wparam);

				// 키 눌림 및 해제 상태 전달.
				Input::Get().SetKeyUpDown(vkCode, isKeyUp, isKeyDown);
			}
		}
		return 0;

		// 마우스 입력 처리
		case WM_LBUTTONDOWN:
		{
			// 예외처리
			if (!Input::IsValid())
			{
				break;
			}

			Input::Get().SetButtonUpDown(0, false, true);
		} return 0;

		case WM_LBUTTONUP:
		{
			// 예외처리
			if (!Input::IsValid())
			{
				break;
			}

			Input::Get().SetButtonUpDown(0, true, false);
		} return 0;

		case WM_RBUTTONDOWN:
		{
			// 예외처리
			if (!Input::IsValid())
			{
				break;
			}

			Input::Get().SetButtonUpDown(1, false, true);
		} return 0;

		case WM_RBUTTONUP:
		{
			// 예외처리
			if (!Input::IsValid())
			{
				break;
			}

			Input::Get().SetButtonUpDown(0, true, false);
		} return 0;

		// 마우스 이동 처리
		case WM_MOUSEMOVE:
		{
			// 예외처리
			if (!Input::IsValid())
			{
				break;
			}

			// 좌표 값 추출 (싱글 모니터만 대응)
			//int xPosition = LOWORD(lparam);
			//int yPosition = HIWORD(lparam);
			int xPosition = GET_X_LPARAM(lparam);
			int yPosition = GET_Y_LPARAM(lparam);

			// 설정
			Input::Get().SetMousePosition(xPosition, yPosition);

		} return 0;

		}
		return DefWindowProc(handle, message, wparam, lparam);
	}

	void Engine::OnInitialized()
	{
		if (!mainLevel || mainLevel->HasInitialized())
		{
			return;
		}

		mainLevel->OnInitialized();
	}

	void Engine::BeginPlay()
	{
		if (!mainLevel)
		{
			return;
		}

		mainLevel->BeginPlay();
	}

	void Engine::Tick(float deltaTime)
	{
		if (!mainLevel)
		{
			return;
		}

		mainLevel->Tick(deltaTime);
	}

	void Engine::Draw()
	{
		if (!mainLevel)
		{
			return;
		}

		graphicsContext->BeginScene(0.6f, 0.7f, 0.8f);

		mainLevel->Draw();
		renderer->DrawScene();

		graphicsContext->EndScene(setting.vsync);
	}
}