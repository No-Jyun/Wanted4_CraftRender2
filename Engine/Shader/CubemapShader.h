#pragma once

#include "Shader.h"
#include <memory>

namespace Craft
{
	class CubemapTexture;
	class CRAFT_API CubemapShader : public Shader
	{
	public : 
		CubemapShader();
		~CubemapShader() = default;

		// 바인드 함수 오버라이드
		virtual void Bind() override;

		// 텍스처 설정 함수
		inline void SetTexture(const std::weak_ptr< CubemapTexture>& newTexture)
		{
			texture = newTexture;
		}

	private:
		std::weak_ptr<CubemapTexture> texture;
	};
}



