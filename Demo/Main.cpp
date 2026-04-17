#include "Core/Engine.h"
#include "Level/TriangleDemoLevel.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace Craft;

// 엔진 실행 함수.
void LaunchEngineStartup(HINSTANCE instance)
{
	Engine engine;
	engine.Initialize(instance);
	engine.AddNewLevel<TriangleDemoLevel>();
	engine.Run();
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	LaunchEngineStartup(GetModuleHandle(nullptr));
}