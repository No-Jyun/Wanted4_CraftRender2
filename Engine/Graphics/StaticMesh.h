#pragma once

#include "Core/Common.h"
#include "Vertex.h"
#include <d3d11.h>
#include <stdint.h>
#include <vector>

namespace Craft
{
	// 서브 메시 구조체
	struct CRAFT_API SubMesh
	{
		SubMesh();
		SubMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~SubMesh();

		// 셰이더 바인딩 함수
		void Bind();

		// 인덱스 수 반환 함수
		inline uint32_t GetIndexCount() const { return static_cast<uint32_t>(indices.size()); }

		std::vector<Vertex> vertices;
		uint32_t stride = 0;
		ID3D11Buffer* vertexBuffer = nullptr;

		std::vector<uint32_t> indices;
		ID3D11Buffer* indexBuffer = nullptr;
	};

	class CRAFT_API StaticMesh
	{
	public:
		StaticMesh();
		virtual ~StaticMesh();

		void Initialize(
			const void* vertices,
			uint32_t vertexCount,
			uint32_t stride,
			const void* indices,
			uint32_t indexCount);

		// @Test: 정점 배열이 변경된 경우 정점 버퍼를 업데이트하는 함수
		void UpdateVertexBuffer(const std::vector<Vertex>& vertices);

		// 셰이더에 연결(바인드) 하는 함수
		virtual void Bind();
		
		inline ID3D11Buffer* GetVertexBuffer() const { return vertexBuffer; }
		inline ID3D11Buffer* GetIndexBuffer() const { return indexBuffer; }
		inline uint32_t GetIndexCount() const { return indexCount; }
		inline uint32_t GetStride() const { return stride; }

		// @Test
		inline std::vector<Vertex> GetVertices() const { return vertices; }

	protected:
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		uint32_t indexCount = 0;
		uint32_t stride = 0;

		// @Test
		std::vector<Vertex> vertices;
	};
}