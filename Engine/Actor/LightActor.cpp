#include "LightActor.h"
#include "Graphics/Renderer.h"
#include "Math/Transform.h"

namespace Craft
{
	LightActor::LightActor()
	{

	}

	void LightActor::Draw()
	{
		Actor::Draw();

		// Todo : 렌더러에 라이트 데이터 전달
		Renderer::Get().UpdateLightData(transform->position, intensity, color);
	}
}