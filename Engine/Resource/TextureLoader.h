#pragma once

#include "Core/Common.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace Craft
{
	class Texture;

	class CRAFT_API TextureLoader
	{
	public:
		TextureLoader();
		~TextureLoader() = default;

		void Load(const std::string& name, std::weak_ptr<Texture>& outTexture);

		static TextureLoader& Get();

	private:
		static TextureLoader* instance;

		std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap;
	};
}

