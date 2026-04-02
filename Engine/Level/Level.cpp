#include "Level.h"
#include "Actor/Actor.h"
#include "Actor/CameraActor.h"

namespace Craft
{
	Level::Level()
	{
	}

	Level::~Level()
	{
	}

	void Level::OnInitialized()
	{
		hasInitialized = true;
	}

	void Level::BeginPlay()
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (actor->HasBeganPlay() || !actor->IsActive())
			{
				continue;
			}

			actor->BeginPlay();
		}
	}

	void Level::Tick(float deltaTime)
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (!actor->IsActive())
			{
				continue;
			}

			actor->Tick(deltaTime);
		}
	}

	void Level::Draw()
	{
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			if (!actor->IsActive())
			{
				continue;
			}

			actor->Draw();
		}
	}

	void Level::OnResize(uint32_t width, uint32_t height)
	{
		// 카메라 액터 찾아서 너비 / 높이 값 전달
		for (const std::shared_ptr<Actor>& actor : actorList)
		{
			// 형변환 시도 및 성공 시 이벤트 전달
			if (auto camera = std::dynamic_pointer_cast<CameraActor>(actor))
			{
				camera->OnResize(width, height);
				break;
			}
		}
	}

	void Level::ProcessAddAndDestroyActors()
	{
		for (auto iterator = actorList.begin();
			iterator != actorList.end();)
		{
			if ((*iterator)->HasExpired())
			{
				iterator = actorList.erase(iterator);
				continue;
			}

			++iterator;
		}

		if (addRequestedActorList.empty())
		{
			return;
		}

		for (const std::shared_ptr<Actor>& actor : addRequestedActorList)
		{
			actorList.emplace_back(actor);
		}

		addRequestedActorList.clear();
	}
}