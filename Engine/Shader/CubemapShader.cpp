#include "CubemapShader.h"
#include "Texture/CubemapTexture.h"

namespace Craft
{
	CubemapShader::CubemapShader()
		: Shader(L"Cubemap")
	{

	}

	void CubemapShader::Bind()
	{
		Shader::Bind();

		// 큐브맵 텍스처 바인딩
		if (texture.lock())
		{
			texture.lock()->Bind();
		}
	}
}