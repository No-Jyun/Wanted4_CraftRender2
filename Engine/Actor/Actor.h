#pragma once

#include "Core/Common.h"
#include <memory>

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

		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && !hasExpired; }
		inline bool HasExpired() const { return hasExpired; }

		void SetOwner(std::weak_ptr<Level> newOwner);

	public:
		// Æ®·£½ºÆû
		std::shared_ptr<Transform> transform;

	protected:
		bool hasBeganPlay = false;
		bool isActive = true;
		bool hasExpired = false;

		std::weak_ptr<StaticMesh> mesh;
		std::shared_ptr<Shader> shader;
		std::weak_ptr<Level> owner;
	};
}