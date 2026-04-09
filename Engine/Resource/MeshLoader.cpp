#include "MeshLoader.h"
#include "Graphics/StaticMesh.h"

#include "Graphics/Vertex.h"

#include <cassert>

#include <fstream>
#include <sstream>

namespace Craft
{
	MeshLoader* MeshLoader::instance = nullptr;

	MeshLoader::MeshLoader()
	{
		assert(!instance);
		instance = this;
	}

	void MeshLoader::GetMesh(
		const std::string& name,
		std::weak_ptr<StaticMesh>& outMesh)
	{
		// 맵에 있는지 확인. 있으면 바로 반환.
		auto result = meshList.find(name);
		if (result != meshList.end())
		{
			outMesh = result->second;
			return;
		}

		// 없는 경우에는 로드해서 반환.
		LoadOBJ(name, outMesh);
	}

	void MeshLoader::LoadOBJ(
		const std::string& name,
		std::weak_ptr<StaticMesh>& outMesh)
	{
		std::string path = std::string("../Assets/Meshes/") + name;
		std::ifstream file(path);
		if (!file.is_open())
		{
			__debugbreak();
			return;
		}

		std::vector<Vector3> positions;
		std::vector<Vector2> texCoords;
		std::vector<Vector3> normals;
		std::vector<Vertex> vertices;

		std::string line;
		while (std::getline(file, line))
		{
			std::string header;
			std::stringstream ss(line);
			ss >> header;
			if (header == "v")
			{
				Vector3 position;
				ss >> position.x >> position.y >> position.z;
				positions.emplace_back(position);
			}
			else if (header == "vt")
			{
				Vector2 texCoord;
				ss >> texCoord.x >> texCoord.y;
				texCoords.emplace_back(texCoord);
			}
			else if (header == "vn")
			{
				Vector3 normal;
				ss >> normal.x >> normal.y >> normal.z;
				normals.emplace_back(normal);
			}
			else if (header == "f")
			{
				int v1, v2, v3;
				int t1, t2, t3;
				int n1, n2, n3;
				sscanf_s(ss.str().c_str(),
					"f %d/%d/%d %d/%d/%d %d/%d/%d",
					&v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);

				vertices.emplace_back(
					positions[v1 - 1], texCoords[t1 - 1], normals[n1 - 1]
				);
				vertices.emplace_back(
					positions[v2 - 1], texCoords[t2 - 1], normals[n2 - 1]
				);
				vertices.emplace_back(
					positions[v3 - 1], texCoords[t3 - 1], normals[n3 - 1]
				);
			}
		}

		// 인덱스 배열 생성.
		std::vector<uint32_t> indices;
		indices.reserve(vertices.size());
		uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
		for (uint32_t ix = 0; ix < vertexCount; ++ix)
		{
			indices.emplace_back(ix);
		}

		// 탄젠트/바이 탄젠트 생성 및 적용.
		const uint32_t count = static_cast<uint32_t>(vertices.size());
		for (uint32_t ix = 0; ix < count; ix += 3)
		{
			// 면을 이루는 3개의 정점 가져오기.
			Vertex& v0 = vertices[ix + 0];
			Vertex& v1 = vertices[ix + 1];
			Vertex& v2 = vertices[ix + 2];

			// 간선 구하기.
			Vector3 edge1 = v1.position - v0.position;
			Vector3 edge2 = v2.position - v0.position;

			// UV 차이 구하기.
			Vector2 deltaUV1 = v1.texCoord - v0.texCoord;
			Vector2 deltaUV2 = v2.texCoord - v0.texCoord;

			// 스케일 구하기.
			float determinant = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			// 탄젠트.
			Vector3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * determinant;
			Vector3 bitangent = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) * determinant;

			v0.tangent = v0.tangent + tangent;
			v1.tangent = v1.tangent + tangent;
			v2.tangent = v2.tangent + tangent;

			v0.bitangent = v0.bitangent + bitangent;
			v1.bitangent = v1.bitangent + bitangent;
			v2.bitangent = v2.bitangent + bitangent;
		}

		// 앞에서 구한 탄젠트/바이 탄젠트/노멀의 직교성 보장하도록 계산.
		// 그람-슈미트 알고리즘 & 외적.
		for (auto& vertex : vertices)
		{
			// 정사영 -> 직교 방향벡터 구하기.
			vertex.tangent = (vertex.tangent - vertex.normal * Dot(vertex.normal, vertex.tangent)).Normalized();
			vertex.tangent = vertex.tangent.Normalized();
			vertex.bitangent = Cross(vertex.normal, vertex.tangent);
		}

		std::shared_ptr<StaticMesh> newMesh = std::make_shared<StaticMesh>();
		newMesh->Initialize(
			vertices.data(), vertexCount, Vertex::Stride(),
			indices.data(), static_cast<uint32_t>(indices.size())
		);

		outMesh = newMesh;
		meshList.insert({ name, newMesh });
		file.close();
	}

	MeshLoader& MeshLoader::Get()
	{
		assert(instance);
		return *instance;
	}
}