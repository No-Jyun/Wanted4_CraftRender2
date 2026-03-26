#include "SpecularMappingShader.h"
#include "Resource/TextureLoader.h"
#include "Texture/Texture.h"

namespace Craft
{
	SpecularMappingShader::SpecularMappingShader(const std::string& diffuseTexture, const std::string& specularTexture)
		: Shader(L"SpecularMapping")
	{
		// 텍스처 로드
		TextureLoader::Get().Load(diffuseTexture, diffuse);
		TextureLoader::Get().Load(specularTexture, specular);
	}

	void SpecularMappingShader::Bind()
	{
		Shader::Bind();

		// 텍스처 추가로 바인딩
		if (diffuse.lock() && specular.lock())
		{
			diffuse.lock()->Bind(0);
			specular.lock()->Bind(1);
		}
	}
}