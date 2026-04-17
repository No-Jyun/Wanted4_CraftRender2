#pragma once

#include "Shader.h"
#include <memory>
#include <string>

namespace Craft
{
	// 전방 선언
	class Texture;

	class CRAFT_API TextureMappingShader : public Shader
	{
	public:
		TextureMappingShader();
		TextureMappingShader(const std::string& textureName);

		virtual void Bind() override;

		// 텍스처 설정 함수
		inline void SetTexture(const std::weak_ptr<Texture>& newTexture) { texture = newTexture; }

	private:
		// 셰이더에 바인딩할 텍스처
		std::weak_ptr<Texture> texture;
	};
}
