#pragma once
#pragma warning(disable: 4251)

#include <Windows.h>
#include <comdef.h>

// D3D 쪽의 리소스 해제할 때 사용할 함수.
template<typename T>
void SafeRelease(T*& resource)
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
}

#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#if defined(ENGINE_BUILD_DLL)
#define CRAFT_API DLLEXPORT
#else
#define CRAFT_API DLLIMPORT
#endif

// DX Call Macro.
#define ThrowIfFailed(result, message)										\
	if (FAILED(result))														\
	{																		\
		_com_error error(result);											\
		const wchar_t* reason = error.ErrorMessage();						\
		const int bufferSize = 2048;										\
		wchar_t errorMessageBuffer[bufferSize] = {};						\
		swprintf_s(errorMessageBuffer, bufferSize,							\
			L"[D3D Error]\n"												\
			L"Message: %s\n"												\
			L"Reason: %s\n"													\
			L"File: %s\n"													\
			L"Line: %d\n"													\
			L"Function: %s\n",												\
			message, reason, TEXT(__FILE__), __LINE__, TEXT(__FUNCTION__));	\
		MessageBoxW(nullptr, errorMessageBuffer, L"D3D Error", MB_OK);		\
		__debugbreak();														\
	}