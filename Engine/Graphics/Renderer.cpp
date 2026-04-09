#include "Renderer.h"
#include "Core/Common.h"
#include "GraphicsContext.h"
#include "StaticMesh.h"
#include "Shader/Shader.h"
#include "Math/Transform.h"
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
		// 카메라 버퍼 리소스 해제
		SafeRelease(cameraBuffer);

		// 라이트 버퍼 리소스 해제
		SafeRelease(lightBuffer);
	}

	// 초기화.
	void Renderer::Initialize()
	{
		// 카메라 행렬 버퍼 생성
		auto& device = GraphicsContext::Get().GetDevice();

		// 버퍼 생성을 위한 구조체
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(CameraData);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// 버퍼 생성
		ThrowIfFailed(
			device.CreateBuffer(&bufferDesc, nullptr, &cameraBuffer),
			L"Failed to Create Camera Buffer.");

		// 라이트 버퍼 생성을 위한 설명 구조체
		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.ByteWidth = sizeof(LightData);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

		// 라이트 버퍼 생성
		ThrowIfFailed(
			device.CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer),
			L"Failed to create light buffer"
		);

	}

	void Renderer::Submit(
		std::shared_ptr<SubMesh> mesh,
		std::shared_ptr<Shader> shader,
		std::shared_ptr<Transform> transform)
	{
		RenderCommand command;
		command.mesh = mesh;
		command.shader = shader;
		command.transform = transform;

		renderQueue.emplace_back(command);
	}

	void Renderer::UpdateCameraMatrix(
		const Matrix4& viewMatrix,
		const Matrix4& projectionMatrix,
		const Vector3& position)
	{
		// 카메라 버퍼 업데이트
		auto& context = GraphicsContext::Get().GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE resource = {};
		ThrowIfFailed(context.Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource),
			L"Failed to map camera buffer.");

		// 카메라 데이터
		CameraData newData;

		// 행렬 전치
		newData.matrix = Matrix4::Transpose(viewMatrix * projectionMatrix);

		// 카메라 위치 저장
		newData.position = position;

		// 데이터 업데이트
		memcpy(resource.pData, &newData, sizeof(CameraData));

		context.Unmap(cameraBuffer, 0);
	}

	void Renderer::UpdateLightData(const Vector3& position, float intensity, const Vector3& color)
	{
		// 라이트 버퍼 업데이트
		auto& context = GraphicsContext::Get().GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE resource = {};
		ThrowIfFailed(context.Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource),
			L"Failed to map light buffer.");

		// 라이트 데이터
		LightData newData;
		newData.position = position;
		newData.intensity = intensity;
		newData.color = color;

		// 데이터 업데이트
		memcpy(resource.pData, &newData, sizeof(LightData));

		context.Unmap(lightBuffer, 0);
	}

	// DrawCall 발생 처리.
	// -> 렌더링 파이프라인 실행(구동).
	void Renderer::DrawScene()
	{
		// 바인딩.
		// -> 셰이더 각 단계에 필요한 정보 전달 및 설정.
		// State 설정.
		auto& context = GraphicsContext::Get().GetDeviceContext();

		// 렌더 커맨드 가져오기.
		for (const RenderCommand& command : renderQueue)
		{
			//auto vertexBuffer = command.mesh->GetVertexBuffer();
			//uint32_t stride = command.mesh->GetStride();
			//uint32_t offset = 0;

			//context.IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			//context.IASetIndexBuffer(command.mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
			//context.IASetInputLayout(command.shader->GetInputLayout());
			//// 점 3개씩 잘라서 읽고, 삼각형을 만들어주는 모드.
			////context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//// 셰이더 설정.
			//context.VSSetShader(command.shader->GetVertexShader(), nullptr, 0);
			//context.PSSetShader(command.shader->GetPixelShader(), nullptr, 0);

			// 메시 바인딩
			command.mesh->Bind();

			// 셰이더 바인딩
			command.shader->Bind();

			// 트랜스폼 바인딩
			command.transform->Bind();

			// 카메라 버퍼 바인딩
			context.VSSetConstantBuffers(1, 1, &cameraBuffer);

			// 라이트 버퍼 바인딩
			context.PSSetConstantBuffers(0, 1, &lightBuffer);

			// 드로우 콜.
			// 렌더링 파이프라인 동작.
			context.DrawIndexed(command.mesh->GetIndexCount(), 0, 0);
		}

		renderQueue.clear();
	}

	Renderer& Renderer::Get()
	{
		assert(instance);
		return *instance;
	}
}