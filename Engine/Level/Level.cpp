#include "Level.h"
#include "Actor/Actor.h"

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