#include "StaticMesh.h"
#include "GraphicsContext.h"
#include "Core/Common.h"

namespace Craft
{
	StaticMesh::StaticMesh()
	{	
	}

	StaticMesh::~StaticMesh()
	{
		SafeRelease(vertexBuffer);
		SafeRelease(indexBuffer);
	}

	void StaticMesh::Initialize(
		const void* vertices, uint32_t vertexCount, uint32_t stride, 
		const void* indices, uint32_t indexCount)
	{
		// @Test: 멤버 변수에 정점 버퍼 저장
		this->vertices.reserve(vertexCount);
		Vertex* vertexArray = (Vertex*)(vertices);
		for (uint32_t i = 0; i < vertexCount; i++)
		{
			this->vertices.emplace_back(vertexArray[i]);
		}

		auto& device = GraphicsContext::Get().GetDevice();

		this->stride = stride;
		this->indexCount = indexCount;

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = vertices;

		vertexBuffer = nullptr;
		HRESULT result = device.CreateBuffer(
			&vertexBufferDesc,
			&vertexData,
			&vertexBuffer
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * indexCount;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		// 서브 리소스 데이터 설정 (실제 데이터 할당).
		D3D11_SUBRESOURCE_DATA indexData = { };
		indexData.pSysMem = indices;

		// 인덱스 버퍼 생성.
		indexBuffer = nullptr;
		result = device.CreateBuffer(
			&indexBufferDesc,
			&indexData,
			&indexBuffer
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}
	}

	void StaticMesh::UpdateVertexBuffer(const std::vector<Vertex>& vertices)
	{
		// 예외처리
		if (!vertexBuffer || vertices.empty())
		{
			return;
		}

		// 업데이트 (CPU -> GPU)
		auto& context = GraphicsContext::Get().GetDeviceContext();
		context.UpdateSubresource(
			vertexBuffer,
			0,
			nullptr,
			vertices.data(),
			0,
			0
		);

		GetLastError();
	}

	void StaticMesh::Bind()
	{
		// DeviceContext 얻어오기
		auto& context = GraphicsContext::Get().GetDeviceContext();

		uint32_t offset = 0;
		context.IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context.IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

