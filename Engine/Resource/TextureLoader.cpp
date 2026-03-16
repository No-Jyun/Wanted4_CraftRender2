#include "TextureLoader.h"
#include "Texture/Texture.h"

#include <cassert>

namespace Craft
{
	TextureLoader* TextureLoader::instance = nullptr;

	TextureLoader::TextureLoader()
	{
		// null 확인 - 싱글톤이어야 하는 객체인데,
		// null이 아닌 경우에는 다른 객체가 이미 있다는 의미
		assert(!instance);
		instance = this;
	}

	void TextureLoader::Load(const std::string& name, std::weak_ptr<Texture>& outTexture)
	{
		// 검색 (맵- 해시테이블)
		auto result = textureMap.find(name);

		// 검색 성공한 경우
		if (result != textureMap.end())
		{
			outTexture = result->second;
			return;
		}

		// 못 찾은 경우 (테이블에 없음)
		// 텍스처 생성 -> 테이블 추가 -> 반환
		std::shared_ptr<Texture> newTexture = std::make_shared<Texture>(name);

		textureMap.insert({ name, newTexture });

		outTexture = newTexture;
	}

	TextureLoader& TextureLoader::Get()
	{
		assert(instance);
		return *instance;
	}
}