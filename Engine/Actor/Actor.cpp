#include "Actor.h"
#include "Graphics/Renderer.h"
#include "Math/Transform.h"

namespace Craft
{
	Actor::Actor()
		: transform(std::make_shared<Transform>())
	{
	}

	Actor::~Actor()
	{
	}

	void Actor::BeginPlay()
	{
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{
		// 트랜스폼 업데이트
		transform->Update();
	}

	void Actor::Draw()
	{
		// @Incomplete: 예외처리
		if (!mesh.lock())
		{
			// 일단 조용히 반환
			// silence is violent
			return;
		}

		Renderer::Get().Submit(mesh.lock(), shader, transform);
	}

	void Actor::SetOwner(std::weak_ptr<Level> newOwner)
	{
		owner = newOwner;
	}
}