#include "Renderer.h"
#include "Core/Common.h"
#include "GraphicsContext.h"
#include "StaticMesh.h"
#include "Shader/Shader.h"
#include "Math/Transform.h"

#include "Resource/TextureLoader.h"
#include "Texture/RenderTexture.h"

#include <d3dcompiler.h>
#include <cassert>

namespace Craft
{
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer()
	{
		assert(!instance);
		instance = this;
	}

	Renderer::~Renderer()
	{
		// 카메라 버퍼 리소스 해제.
		SafeRelease(cameraBuffer);

		// 라이트 버퍼 리소스 해제.
		SafeRelease(lightBuffer);

		// RSState 해제.
		SafeRelease(cullBackRSState);
		SafeRelease(cullFrontRSState);
	}

	// 초기화.
	void Renderer::Initialize()
	{
		// 카메라 행렬 버퍼 생성.
		auto& device = GraphicsContext::Get().GetDevice();

		// 버퍼 생성을 위한 설명 구조체.
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(CameraData);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// 버퍼 생성.
		ThrowIfFailed(
			device.CreateBuffer(&bufferDesc, nullptr, &cameraBuffer),
			L"Failed to create camera buffer");

		// 라이트 버퍼 생성을 위한 설명 구조체.
		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.ByteWidth = sizeof(LightData);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// 라이트 버퍼 생성.
		ThrowIfFailed(
			device.CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer),
			L"Failed to create light buffer"
		);

		// 래스터라이저 스테이트 객체 생성.
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.DepthClipEnable = true;

		ThrowIfFailed(
			device.CreateRasterizerState(&rasterizerDesc, &cullBackRSState),
			L"Failed to create cull back rs state");

		rasterizerDesc.CullMode = D3D11_CULL_FRONT;

		ThrowIfFailed(
			device.CreateRasterizerState(&rasterizerDesc, &cullFrontRSState),
			L"Failed to create cull front rs state");
	}

	void Renderer::Submit(
		std::shared_ptr<SubMesh> mesh,
		std::shared_ptr<Shader> shader,
		std::shared_ptr<Transform> transform,
		bool isSkybox,
		bool isUsingRenderTexture)
	{
		RenderCommand command;
		command.mesh = mesh;
		command.shader = shader;
		command.transform = transform;
		command.isSkybox = isSkybox;
		command.isUsingRenderTexture = isUsingRenderTexture;

		renderQueue.emplace_back(command);
	}

	void Renderer::UpdateCameraMatrix(
		const Matrix4& viewMatrix,
		const Matrix4& projectionMatrix,
		const Vector3& position)
	{
		// 카메라 버퍼 업데이트.
		auto& context = GraphicsContext::Get().GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE resource = {};
		ThrowIfFailed(
			context.Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource),
			L"Failed to map camera buffer");

		// 카메라 데이터.
		CameraData newData;

		// 행렬 전치.
		newData.matrix
			= Matrix4::Transpose(viewMatrix * projectionMatrix);

		// 카메라 위치 저장.
		newData.position = position;

		// 데이터 업데이트.
		memcpy(resource.pData, &newData, sizeof(CameraData));

		context.Unmap(cameraBuffer, 0);
	}

	void Renderer::UpdateLightData(
		const Vector3& position,
		float intensity,
		const Vector3& color)
	{
		// 라이트 버퍼 업데이트.
		auto& context = GraphicsContext::Get().GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE resource = {};
		ThrowIfFailed(
			context.Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource),
			L"Failed to map light buffer");

		// 라이트 데이터.
		LightData newData;
		newData.position = position;
		newData.intensity = intensity;
		newData.color = color;

		// 데이터 업데이트.
		memcpy(resource.pData, &newData, sizeof(LightData));

		context.Unmap(lightBuffer, 0);
	}

	// DrawCall 발생 처리.
	// -> 렌더링 파이프라인 실행(구동).
	void Renderer::DrawScene()
	{
		// 렌더 텍스처에 그리는 패스 실행.
		DrawToRenderTexturePass();

		// 씬 렌더링 패스(백버퍼에 씬 그리기).
		DrawScenePass();

		renderQueue.clear();
	}

	Renderer& Renderer::Get()
	{
		assert(instance);
		return *instance;
	}

	void Renderer::DrawToRenderTexturePass()
	{
		auto& context = GraphicsContext::Get().GetDeviceContext();

		// 렌더 텍스처를 순회하면서 드로우 처리
		const uint32_t count = TextureLoader::Get().GetRenderTextureCount();
		for (uint32_t ix = 0; ix < count; ix++)
		{
			auto renderTexture = TextureLoader::Get().GetRenderTexture(ix);
			// 클리어 색상
			float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };

			// RTV 임시로 저장
			auto* renderTargetView = renderTexture->GetRenderTargetView();

			// OM 단계에서 RTV / DSV 바인딩
			context.OMSetRenderTargets(1, &renderTargetView, renderTexture->GetDepthStencilView());
			//GetLastError();

			// 클리어 처리
			context.ClearRenderTargetView(renderTargetView, color);
			context.ClearDepthStencilView(renderTexture->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			// 드로우
			for (const RenderCommand& command : renderQueue)
			{
				// 렌더 텍스처는 건너뛰기
				if (command.isUsingRenderTexture)
				{
					continue;
				}

				// 스카이 박스 여부 확인.
				if (command.isSkybox)
				{
					CullFront();
				}

				// 메시 바인딩.
				command.mesh->Bind();

				// 셰이더 바인딩.
				command.shader->Bind();

				// 트랜스폼 바인딩.
				command.transform->Bind();

				// 카메라 버퍼 바인딩.
				context.VSSetConstantBuffers(1, 1, &cameraBuffer);

				// 라이트 버퍼 바인딩.
				context.PSSetConstantBuffers(0, 1, &lightBuffer);

				// 드로우 콜.
				// 렌더링 파이프라인 동작.
				context.DrawIndexed(
					command.mesh->GetIndexCount(), 0, 0
				);

				// RSState 원상 복구.
				CullBack();
			}
		}
	}

	void Renderer::DrawScenePass()
	{
		// 원래 RTV/DSV 설정 및 클리어 처리
		GraphicsContext::Get().BeginScene(0.6f, 0.7f, 0.8f);

		// 바인딩.
		// -> 셰이더 각 단계에 필요한 정보 전달 및 설정.
		// State 설정.
		auto& context = GraphicsContext::Get().GetDeviceContext();

		// 렌더 커맨드 가져오기.
		for (const RenderCommand& command : renderQueue)
		{
			// 스카이 박스 여부 확인.
			if (command.isSkybox)
			{
				CullFront();
			}

			// 메시 바인딩.
			command.mesh->Bind();

			// 셰이더 바인딩.
			command.shader->Bind();

			// 트랜스폼 바인딩.
			command.transform->Bind();

			// 카메라 버퍼 바인딩.
			context.VSSetConstantBuffers(1, 1, &cameraBuffer);

			// 라이트 버퍼 바인딩.
			context.PSSetConstantBuffers(0, 1, &lightBuffer);

			// 드로우 콜.
			// 렌더링 파이프라인 동작.
			context.DrawIndexed(
				command.mesh->GetIndexCount(), 0, 0
			);

			// RSState 원상 복구.
			CullBack();
		}
	}

	void Renderer::CullBack()
	{
		auto& context = GraphicsContext::Get().GetDeviceContext();
		context.RSSetState(cullBackRSState);
	}

	void Renderer::CullFront()
	{
		auto& context = GraphicsContext::Get().GetDeviceContext();
		context.RSSetState(cullFrontRSState);
	}
}