#pragma once

#include "Core/Common.h"
#include <Windows.h>
#include <memory>
#include <string>

namespace Craft
{
	// 전방선언.
	class Win32Window;
	class GraphicsContext;
	class Level;
	class MeshLoader;
	class TextureLoader;

	// 엔진 설정.
	struct EngineSetting
	{
		uint32_t width = 1280;
		uint32_t height = 800;
		std::wstring title = L"Craft Engine";
		float framerate = 120.0f;
		uint32_t vsync = 0;
	};

	class CRAFT_API Engine
	{
	public:
		Engine();
		virtual ~Engine();

		// 초기화 함수.
		bool Initialize(HINSTANCE instance);

		// 엔진 루프 실행 함수.
		void Run();

		template<typename T, typename ...Args, 
			typename = std::enable_if_t<std::is_base_of<Level, T>::value>>
		void AddNewLevel(Args&&... args)
		{
			nextLevel = std::make_shared<T>(std::forward<Args>(args)...);
		}

	protected:
		// Win32 윈도우 메시지 처리 함수(콜백 함수).
		static LRESULT CALLBACK Win32MessageProcedure(
			HWND handle,
			UINT message,
			WPARAM wparam,
			LPARAM lparam
		);

		void OnInitialized();
		void BeginPlay();
		void Tick(float deltaTime);
		void Draw();

	protected:
		// 창 객체.
		std::unique_ptr<Win32Window> window;

		// 그래픽스 컨텍스트 객체 (장치 관리 등등).
		std::unique_ptr<GraphicsContext> graphicsContext;

		// 렌더러 객체(장면 그리기 담당).
		std::unique_ptr<class Renderer> renderer;

		// 메시 로더 객체 (메시 관리)
		std::unique_ptr<MeshLoader> meshLoader;

		// 텍스처 로더 객체 (텍스처 관리)
		std::unique_ptr<TextureLoader> textureLoader;

		std::shared_ptr<Level> mainLevel;

		std::shared_ptr<Level> nextLevel;

		// 엔진 설정 변수.
		EngineSetting setting;
	};
}