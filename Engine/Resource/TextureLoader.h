#pragma once

#include "Core/Common.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <cassert>

namespace Craft
{
	class Texture;
	class CubemapTexture;
	class RenderTexture;

	class CRAFT_API TextureLoader
	{
	public:
		TextureLoader();
		~TextureLoader() = default;

		void Load(
			const std::string& name,
			std::weak_ptr<Texture>& outTexture);

		// 큐브맵 로드 함수.
		void LoadCubemap(
			const std::string& path,
			std::weak_ptr<CubemapTexture>& outTexture
		);

		// 렌더 텍스처 반환 함수.
		void GetNewRenderTexture(
			std::weak_ptr<RenderTexture>& outTexture,
			uint32_t width = 0, uint32_t height = 0);

		// Getter.
		inline uint32_t GetRenderTextureCount() const
		{
			return static_cast<uint32_t>(renderTextureList.size());
		}

		inline std::shared_ptr<RenderTexture> GetRenderTexture(uint32_t index)
		{
			assert(index >= 0 && index < (uint32_t)renderTextureList.size());
			return renderTextureList[index];
		}

		static TextureLoader& Get();

	private:

		static TextureLoader* instance;

		std::unordered_map<
			std::string,
			std::shared_ptr<Texture>> textureMap;

		// 큐브맵 관리용 맵.
		std::unordered_map<
			std::string,
			std::shared_ptr<CubemapTexture>> cubemapTextureMap;

		// 렌더 텍스처 배열.
		std::vector<std::shared_ptr<RenderTexture>> renderTextureList;
	};
}