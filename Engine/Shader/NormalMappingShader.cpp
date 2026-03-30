#include "NormalMappingShader.h"
#include "Resource/TextureLoader.h"
#include "Texture/Texture.h"

namespace Craft
{
	NormalMappingShader::NormalMappingShader(
		const std::string& diffuseTexture, 
		const std::string& specularTexture, const std::string& normalTexture)
		: Shader(L"NormalMapping")
	{
		// 텍스처 로드.
		TextureLoader::Get().Load(diffuseTexture, diffuse);
		TextureLoader::Get().Load(specularTexture, specular);
		TextureLoader::Get().Load(normalTexture, normal);
	}

	void NormalMappingShader::Bind()
	{
		Shader::Bind();

		if (diffuse.lock() && specular.lock() && normal.lock())
		{
			diffuse.lock()->Bind(0);
			specular.lock()->Bind(1);
			normal.lock()->Bind(2);
		}
	}
}