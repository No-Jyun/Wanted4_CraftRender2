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
		TextureMappingShader(const std::string& textureName);

		virtual void Bind() override;

	private:
		// 셰이더에 바인딩할 텍스처
		std::weak_ptr<Texture> texture;
	};
}
