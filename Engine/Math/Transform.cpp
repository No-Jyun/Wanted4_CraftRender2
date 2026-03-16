#include "Transform.h"
#include "Graphics/GraphicsContext.h"

namespace Craft
{
	Transform::Transform()
	{
		// 행렬 만들기 ( S -> R -> T )
		// 회전 후 이동과 이동 후 회전은 다름
		transformMatrix = 
			Matrix4::Scale(scale) * 
			Matrix4::Rotation(rotation) * 
			Matrix4::Translation(position);

		// 전치 (CPU는 행 기준, GPU는 열 기준)
		transformMatrix = Matrix4::Transpose(transformMatrix);

		// 상수 버퍼 생성
		auto& device = GraphicsContext::Get().GetDevice();

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(Matrix4);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;		// 프레임마다 빈번하게 변경되는 경우
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA bufferData = {};
		bufferData.pSysMem = &transformMatrix;

		HRESULT result = device.CreateBuffer(
			&bufferDesc,
			&bufferData,
			&constantBuffer
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}
	}

	Transform::~Transform()
	{
		// 리소스 해제
		SafeRelease(constantBuffer);
	}

	void Transform::Update()
	{
		// 행렬 만들기 ( S -> R -> T )
		// 회전 후 이동과 이동 후 회전은 다름
		transformMatrix =
			Matrix4::Scale(scale) *
			Matrix4::Rotation(rotation) *
			Matrix4::Translation(position);

		// 전치 (CPU는 행 기준, GPU는 열 기준)
		transformMatrix = Matrix4::Transpose(transformMatrix);
	}

	void Transform::Bind()
	{
		auto& context = GraphicsContext::Get().GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
		// mappedSubresource 처리를 위해 락 (lock)
		HRESULT result = context.Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		// 데이터 업데이트
		memcpy(mappedSubresource.pData, &transformMatrix, sizeof(Matrix4));

		// 락 해제
		context.Unmap(constantBuffer, 0);

		// 바인딩
		// 정점 셰이더에 바인딩
		context.VSSetConstantBuffers(0, 1, &constantBuffer);
	}
}