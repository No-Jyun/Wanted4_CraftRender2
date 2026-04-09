#pragma once

#include "Core/Common.h"
#include "Vertex.h"
#include <d3d11.h>
#include <stdint.h>
#include <vector>
#include <memory>

namespace Craft
{
	// 서브 메시 구조체
	struct CRAFT_API SubMesh
	{
		SubMesh();
		SubMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~SubMesh();

		// 서브 메쉬 바인딩 함수
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

		// 서브 메쉬 추가 함수
		void AddSubMesh(
			const std::vector<Vertex>& vertices,
			const std::vector<uint32_t>& indices
		);

		// 셰이더에 연결(바인드) 하는 함수
		virtual void Bind(uint32_t index = 0);
		
		// 서브 메쉬 반환 함수
		inline std::shared_ptr<SubMesh> GetSubMesh(uint32_t index) { return submeshList[index]; }

		// 서브 메쉬 개수 반환 함수
		inline uint32_t GetSubMeshCount() const { return static_cast<int>(submeshList.size()); }

	protected:
		// 서브 메시 목록
		std::vector<std::shared_ptr<SubMesh>> submeshList;
	};
}