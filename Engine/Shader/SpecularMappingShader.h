#pragma once

#include "Shader.h"
#include <memory>
#include <string>

namespace Craft
{
	// 전방 선언
	class Texture;

	class CRAFT_API SpecularMappingShader : public Shader
	{
	public:
		SpecularMappingShader(const std::string& diffuseTexture, const std::string& specularTexture);

		virtual void Bind() override;

	private:
		// 셰이더에 바인딩할 디퓨즈 텍스처
		std::weak_ptr<Texture> diffuse;

		// 셰이더에 바인딩할 스페큘러 텍스처
		std::weak_ptr<Texture> specular;
	};
}
