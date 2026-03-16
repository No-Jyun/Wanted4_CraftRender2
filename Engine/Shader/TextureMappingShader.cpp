#include "TextureMappingShader.h"
#include "Texture/Texture.h"
#include "Resource/TextureLoader.h"

namespace Craft
{
	TextureMappingShader::TextureMappingShader(const std::string& textureName)
		: Shader(L"TextureMapping")
	{
		// 텍스처 로드
		TextureLoader::Get().Load(textureName, texture);
	}

	void TextureMappingShader::Bind()
	{
		Shader::Bind();

		// 텍스처를 추가로 바인딩
		if(texture.lock())
		{
			texture.lock()->Bind();
		}
	}
}