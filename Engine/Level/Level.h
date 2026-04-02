#pragma once

#include "Core/Common.h"
#include "Actor/Actor.h"
#include <vector>
#include <memory>

namespace Craft
{
	class CRAFT_API Level : public std::enable_shared_from_this<Level>
	{
	public:
		Level();
		virtual ~Level();

		virtual void OnInitialized();
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 화면 변경 함수 -> 카메라 액터에 이벤트 전달
		void OnResize(uint32_t width, uint32_t height);

		void ProcessAddAndDestroyActors();

		inline bool HasInitialized() { return hasInitialized; }

		template<typename T, typename ...Args,
			typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>
		std::shared_ptr<T> SpawnActor(Args&&... args)
		{
			std::shared_ptr<T> newActor = std::make_shared<T>(std::forward<Args>(args)...);
			addRequestedActorList.emplace_back(newActor);
			newActor->SetOwner(shared_from_this());

			return newActor;
		}

	protected:
		bool hasInitialized = false;
		std::vector<std::shared_ptr<Actor>> actorList;
		std::vector<std::shared_ptr<Actor>> addRequestedActorList;
	};
}