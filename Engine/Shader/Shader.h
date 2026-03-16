#pragma once

#include "Core/Common.h"
#include <d3d11.h>
#include <string>

namespace Craft
{
	// 로우 레벨에선 머티리얼 = 셰이더
	// 관념적으로 머티리얼 = 셰이더 + 속성 (숫자값, 텍스처..)
	class CRAFT_API Shader
	{
	public:
		Shader(const std::wstring& name);
		virtual ~Shader();

		// 바인딩
		virtual void Bind();

		inline ID3D11VertexShader* GetVertexShader() const { return vertexShader; }
		inline ID3D11PixelShader* GetPixelShader() const { return pixelShader; }
		inline ID3D11InputLayout* GetInputLayout() const { return inputLayout; }

	protected:
		std::wstring name;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;
	};
}