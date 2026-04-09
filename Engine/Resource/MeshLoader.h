#pragma once

#include "Core/Common.h"
#include <memory>			// 스마트 포인터
#include <unordered_map>	// 해시 테이블
#include <string>

namespace Craft
{
	// 전방 선언
	class StaticMesh;

	class CRAFT_API MeshLoader
	{
	public:
		MeshLoader();
		~MeshLoader() = default;

		// 메시 요청 함수
		void GetMesh(const std::string& name, std::weak_ptr<StaticMesh>& outMesh);

		static MeshLoader& Get();

	private:
		// 메시 로드 함수
		void LoadOBJ(const std::string& name, std::weak_ptr<StaticMesh>& outMesh);

	private:
		// 맵 (메시 관리 컨테이너)
		std::unordered_map<std::string, std::shared_ptr<StaticMesh>> meshList;
	
		static MeshLoader* instance;
	};
}
