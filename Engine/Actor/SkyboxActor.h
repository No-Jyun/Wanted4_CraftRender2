#pragma once

#include "Actor.h"
#include <string>
namespace Craft
{
	class CRAFT_API SkyboxActor : public Actor
	{
	public:
		SkyboxActor();
		SkyboxActor(const std::string& path);
		~SkyboxActor() = default;
	};
}


