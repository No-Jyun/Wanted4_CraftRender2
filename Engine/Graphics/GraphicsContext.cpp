#include "GraphicsContext.h"
#include "Core/Win32Window.h"
#include "Core/Common.h"
#include <cassert>

namespace Craft
{
	// 전역 변수 초기화.
	GraphicsContext* GraphicsContext::instance = nullptr;

	GraphicsContext::GraphicsContext()
	{
		assert(!instance);
		instance = this;
	}

	GraphicsContext::~GraphicsContext()
	{
		// 자원 해제.
		SafeRelease(device);
		SafeRelease(context);
		SafeRelease(swapChain);
		SafeRelease(renderTargetView);
		SafeRelease(depthStencilView);
	}

	void GraphicsContext::Initialize(const Win32Window& window)
	{
		// 멤버 변수 설정.
		width = window.Width();
		height = window.Height();

		// 장치 생성.
		CreateDevice();

		// SwapChain 생성.
		CreateSwapChain(window);
		
		// 뷰포트 생성.
		CreateViewport(window);

		// 렌더 타겟 뷰 생성.
		CreateRenderTargetView();

		// 뎁스 스텐실 뷰 생성
		CreateDepthStencilView(width, height);

		// @Incomplete: 우리 엔진에서는 뷰포트를 바꿀 필요가 없음.
		context->RSSetViewports(1, &viewport);
	}

	void GraphicsContext::BeginScene(float red, float green, float blue)
	{
		// 그릴 이미지 준비.
		// 한 색으로 이미지를 칠하기.
		// 렌더링 과정을 단순하게: 빈 도화지 준비 -> 그리기 -> 모니터로 전달.
		
		// 그릴 도화지 설정.
		context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

		// 빈도화지로 만들기 -> 한 색상으로 덧칠하기.
		// 사용할 배경색.
		float backgroundColor[4] = { red, green, blue, 1.0f };
		context->ClearRenderTargetView(renderTargetView, backgroundColor);

		// 뎁스 스텐실 뷰 초기화
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	}

	void GraphicsContext::EndScene(uint32_t vsync)
	{
		// 모니터에 전달 (백버퍼-프론트버퍼 교환).
		swapChain->Present(vsync, 0);
	}

	void GraphicsContext::OnResize(uint32_t width, uint32_t height)
	{
		// 예외처리
		if (!device || !context || !swapChain)
		{
			return;
		}

		// 컨텍스트 비우기
		context->ClearState();
		context->Flush();

		// 렌더 타겟 뷰 해제
		SafeRelease(renderTargetView);

		// 뎁스 스텐실 뷰 해체
		SafeRelease(depthStencilView);

		// 스왑 체인 크기 재조정
		ThrowIfFailed(
			swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0),
			L"Failed to resize swapchain buffer"
		);

		// 렌더 타겟 재생성
		CreateRenderTargetView();

		// 뎁스 스텐실 뷰 재생성
		CreateDepthStencilView(width, height);

		// 뷰포트 업데이트
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);

		// 뷰포트 설정
		context->RSSetViewports(1, &viewport);
	}

	GraphicsContext& GraphicsContext::Get()
	{
		assert(instance);
		return *instance;
	}

	void GraphicsContext::CreateDevice()
	{
		// 플래그 지정.
		uint32_t flag = 0;

#if _DEBUG
		flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Direct3D API 버전 정보.
		// 그래픽카드가 지원하는지 확인하는 용도.
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};

		// 장치 생성.
		// Device / DeviceContext 생성.
		/*
		*   IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HMODULE Software,
			UINT Flags,
			CONST D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT FeatureLevels,
			UINT SDKVersion,
			ID3D11Device** ppDevice,
			D3D_FEATURE_LEVEL* pFeatureLevel,
			ID3D11DeviceContext** ppImmediateContext );
		*/
		// 오류인 경우 음수값 반환됨.
		HRESULT result = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flag,
			featureLevels,
			_countof(featureLevels),
			D3D11_SDK_VERSION,
			&device,
			nullptr,
			&context
		);

		// 예외처리.
		//SUCCEEDED()
		if (FAILED(result))
		{
			__debugbreak();
			return;
		}
	}

	void GraphicsContext::CreateSwapChain(const Win32Window& window)
	{
		// 스왑체인 생성해주는 객체 얻어오기.
		IDXGIFactory* factory = nullptr;
		HRESULT result = CreateDXGIFactory(
			__uuidof(IDXGIFactory),
			reinterpret_cast<void**>(&factory)
		);

		// 예외 처리.
		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		// 스왑체인 생성을 위한 자료 설정.
		/*
		*   DXGI_MODE_DESC BufferDesc;
			DXGI_SAMPLE_DESC SampleDesc;
			DXGI_USAGE BufferUsage;
			UINT BufferCount;
			HWND OutputWindow;
			BOOL Windowed;
			DXGI_SWAP_EFFECT SwapEffect;
			UINT Flags;
		*/
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.Windowed = true;	// 창 모드로 시작.
		swapChainDesc.OutputWindow = window.Handle();
		// 화면 출력용.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		// 사용할 버퍼 수.
		swapChainDesc.BufferCount = 2;

		// 안티-안리아싱(Anti-Aliasing)에 수퍼샘플링 수준 설정.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// 버퍼(프레임-이미지) 설정.
		swapChainDesc.BufferDesc.Width = window.Width();
		swapChainDesc.BufferDesc.Height = window.Height();
		// 이미지 픽셀 포맷(32비트-부호없고-정규화된 포맷).
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// 버퍼 교환 효과 설정.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// 스왑 체인 생성.
		result = factory->CreateSwapChain(
			device,
			&swapChainDesc,
			&swapChain
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		// 팩토리 객체 해제.
		SafeRelease(factory);
	}

	void GraphicsContext::CreateViewport(const Win32Window& window)
	{
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(window.Width());
		viewport.Height = static_cast<float>(window.Height());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
	}

	void GraphicsContext::CreateRenderTargetView()
	{
		// 정석적인 방법.
		// 이미지 속성 구조체 설정.
		// 이걸 기반으로 생성.

		// 백버퍼용 렌더타겟뷰 생성.
		// 스왑체인에서 기본 백버퍼 가지고 있음.
		// 스왑체인에서 버퍼 불러와서 생성(똑같은 모양으로).

		// 스왑체인에서 얻어올 백버퍼 정보를 저장할 변수.
		ID3D11Texture2D* backbuffer = nullptr;
		HRESULT result = swapChain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

		// 예외처리.
		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		// RTV 생성.
		result = device->CreateRenderTargetView(
			backbuffer, nullptr, &renderTargetView
		);

		// 예외처리.
		if (FAILED(result))
		{
			// 사용한 리소스 해제.
			SafeRelease(backbuffer);

			__debugbreak();
			return;
		}

		// 사용한 리소스 해제.
		SafeRelease(backbuffer);
	}

	void GraphicsContext::CreateDepthStencilView(uint32_t width, uint32_t height)
	{
		// 2차원 텍스처 생성 -> 뎁스 스텐실 뷰 생성
		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* depthStencilTexture = nullptr;
		ThrowIfFailed(
			device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture),
			L"Failed to create depth stencil texture");

		// 뎁스 스텐실 뷰 생성
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &depthStencilView),
			L"Failed to create depth stencil view");

		// 사용한 리소스 해제
		SafeRelease(depthStencilTexture);
	}
}