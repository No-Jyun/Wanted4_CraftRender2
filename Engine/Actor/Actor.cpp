#include "Actor.h"
#include "Graphics/Renderer.h"
#include "Math/Transform.h"
#include "Core/Engine.h"
#include "Graphics/StaticMesh.h"

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

		// 서브 메시 모두 제출
		const uint32_t subMeshCount = mesh.lock()->GetSubMeshCount();
		
		// 예외처리
		if (subMeshCount != static_cast<uint32_t>(shaderList.size()))
		{
			return;
		}

		for (uint32_t ix = 0; ix < subMeshCount; ix++)
		{
			Renderer::Get().Submit(mesh.lock()->GetSubMesh(ix), shaderList[ix], transform);
		}

		//Renderer::Get().Submit(mesh.lock(), shader, transform);
	}

	void Actor::QuitGame()
	{
		// 엔진 종료 요청
		Engine::Get().Quit();
	}

	void Actor::SetOwner(std::weak_ptr<Level> newOwner)
	{
		owner = newOwner;
	}
}