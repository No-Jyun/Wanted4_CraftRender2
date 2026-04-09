#include "MeshLoader.h"
#include "Core/Common.h"
#include "Graphics/StaticMesh.h"

#include "Graphics/Vertex.h"

#include <cassert>

#include <fstream>
#include <sstream>

// ASSIMP
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include <cimport.h>

namespace Craft
{
	MeshLoader* MeshLoader::instance = nullptr;

	// FBX 메시 처리 함수
	void ProcessMesh(aiMesh* mesh, std::shared_ptr<StaticMesh>& outMesh);	

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

		// 포맷 구분 (파일의 확장자 기반으로)
		auto index = name.find_last_of(".");
		// 확장자만 자르기
		std::string extension = name.substr(index + 1);

		// OBJ / FBX 구분 처리
		if (extension == "obj" || extension == "OBJ")
		{
			// 없는 경우에는 로드해서 반환.
			LoadOBJ(name, outMesh);
		}
		else if (extension == "fbx" || extension == "FBX")
		{
			// 없는 경우에는 로드해서 반환.
			LoadFBX(name, outMesh);
		}
		else
		{
			// 예외처리
			ThrowIfFailed(E_FAIL, L"Craft engine only supports obj or fbx format");
		}
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
		//newMesh->Initialize(
		//	vertices.data(), vertexCount, Vertex::Stride(),
		//	indices.data(), static_cast<uint32_t>(indices.size())
		//);

		newMesh->AddSubMesh(vertices, indices);

		outMesh = newMesh;
		meshList.insert({ name, newMesh });
		file.close();
	}

	void MeshLoader::LoadFBX(const std::string& name, std::shared_ptr<StaticMesh>& outMesh)
	{
		std::string path = std::string("../Assets/Meshes/") + name;
		
		// fbx scene 열기
		const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

		// 확인 (메시가 있는지 확인)
		if (!scene || scene->HasMeshes())
		{
			ThrowIfFailed(E_FAIL, L"Failed to open fbx file or fbx has no mesh");
			return;
		}

		// 서브 메쉬 순환하면서 처리 진행
		for (uint32_t ix = 0; ix < scene->mNumMeshes; ix++)
		{
			ProcessMesh(scene->mMeshes[ix], outMesh);
		}

		// 해제
		aiReleaseImport(scene);
	}

	MeshLoader& MeshLoader::Get()
	{
		assert(instance);
		return *instance;
	}

	void ProcessMesh(aiMesh* mesh, std::shared_ptr<StaticMesh>& outMesh)
	{
		std::vector<Vertex> vertices;
		vertices.reserve(static_cast<uint32_t>(mesh->mNumVertices));
		for (uint32_t ix = 0; ix < mesh->mNumVertices; ++ix)
		{
			const aiVector3D& aiVertex = mesh->mVertices[ix];
			Vector3 position(aiVertex.x, aiVertex.y, aiVertex.z);

			Vector2 texCoord;
			if (mesh->HasTextureCoords(0))
			{
				const aiVector3D& aiTexCoord = mesh->mTextureCoords[0][ix];
				texCoord.x = aiTexCoord.x;
				texCoord.y = aiTexCoord.y;
			}

			Vector3 normal;
			if (mesh->HasNormals())
			{
				const aiVector3D& aiNormal = mesh->mNormals[ix];
				normal.x = aiNormal.x;
				normal.y = aiNormal.y;
				normal.z = aiNormal.z;
			}

			Vector3 tangent;
			Vector3 bitangent;
			if (mesh->HasTangentsAndBitangents())
			{
				const aiVector3D& aiTangent = mesh->mTangents[ix];
				const aiVector3D& aiBitangent = mesh->mBitangents[ix];

				tangent.x = aiTangent.x;
				tangent.y = aiTangent.y;
				tangent.z = aiTangent.z;

				bitangent.x = aiBitangent.x;
				bitangent.y = aiBitangent.y;
				bitangent.z = aiBitangent.z;
			}
			// 탄젠트, 바이 탄젠트가 없다면 노멀 매핑이 안됨
			// 따라서 위 항목은 반드시 있어야함.
			// 그래서 만약에 없다면 LoadOBJ에서 한 작업 해야함

			Vertex vertex(position, texCoord, normal);
			vertex.tangent = tangent;
			vertex.bitangent = bitangent;

			vertices.emplace_back(vertex);
		}

		std::vector<uint32_t> indices;
		indices.reserve(mesh->mNumFaces * 3);
		for (uint32_t ix = 0; ix < mesh->mNumFaces; ++ix)
		{
			const aiFace& face = mesh->mFaces[ix];

			indices.emplace_back(face.mIndices[0]);
			indices.emplace_back(face.mIndices[1]);
			indices.emplace_back(face.mIndices[2]);
		}

		outMesh->AddSubMesh(vertices, indices);
	}
}