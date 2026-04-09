#pragma once

#include "Core/Common.h"
#include <memory>
#include <vector>

namespace Craft
{
	class StaticMesh;
	class Shader;
	class Level;
	class Transform;

	class CRAFT_API Actor
	{
	public:
		Actor();
		virtual ~Actor();

		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 게임 종료 요청 함수
		void QuitGame();

		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && !hasExpired; }
		inline bool HasExpired() const { return hasExpired; }

		void SetOwner(std::weak_ptr<Level> newOwner);

	public:
		// 트랜스폼
		std::shared_ptr<Transform> transform;

	protected:
		bool hasBeganPlay = false;
		bool isActive = true;
		bool hasExpired = false;

		std::weak_ptr<StaticMesh> mesh;
		std::vector<std::shared_ptr<Shader>> shaderList;
		std::weak_ptr<Level> owner;
	};
}