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

	void MeshLoader::GetMesh(const std::string& name, std::weak_ptr<StaticMesh>& outMesh)
	{
		// 맵에 있는지 확인
		// 있으면 바로 반환
		auto result = meshList.find(name);
		if (result != meshList.end())
		{
			outMesh = result->second;
			return;
		}

		// 없는 경우에는 로드해서 반환
		LoadMesh(name, outMesh);
	}
	
	void MeshLoader::LoadMesh(const std::string& name, std::weak_ptr<StaticMesh>& outMesh)
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

				vertices.emplace_back(positions[v1 - 1], texCoords[t1 - 1], normals[n1-1]);
				vertices.emplace_back(positions[v2 - 1], texCoords[t2 - 1], normals[n2-1]);
				vertices.emplace_back(positions[v3 - 1], texCoords[t3 - 1], normals[n3-1]);
			}
		}

		std::vector<uint32_t> indices;
		indices.reserve(vertices.size());
		uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
		for (uint32_t ix = 0; ix < vertexCount; ++ix)
		{
			indices.emplace_back(ix);
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