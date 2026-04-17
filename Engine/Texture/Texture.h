#pragma once
#include "Core/Common.h"
#include <string>
#include <memory>
#include <d3d11.h>

namespace Craft
{
	// 텍스처 데이터
	struct TextureData
	{
		TextureData() = default;
		~TextureData()
		{
			free(pixelArray);
			pixelArray = nullptr;
		}

		int width = 0;
		int height = 0;
		int channelCount = 0;
		void* pixelArray = nullptr;
	};

	// 셰이더에 전달할 텍스처 관련 데이터를 관리
	class CRAFT_API Texture
	{
		// 어느 셰이더에 바인딩할 지를 나타냄
		enum class BindType
		{
			VertexShader,
			PixelShader
		};
	
	public:
		Texture() = default;
		Texture(const std::string& name, BindType bindType = BindType::PixelShader);
		virtual ~Texture();

		// 세이더에 바인딩 (Binding)
		// 설계 관점에서 이 함수가 어디에 있는게 맞는지 고민이 필요
		void Bind(uint32_t index = 0);

	protected:
		// 24비트 텍스처를 32bit 텍스처로 변환하는 함수
		void ConvertToRGBA(std::unique_ptr<TextureData>& textureData);

		// 텍스처 로드 함수.
		virtual void LoadTexture(const std::string& name);

		// 셰이더 리소스 뷰 & 샘플러 스테이트 생성 함수.
		virtual void CreateSRVAndSampler();

	protected:
		// 텍스처 이름
		std::string name;

		// 바인드 타입 지정
		BindType bindType = BindType::PixelShader;
		
		// 텍스처 원시 데이터
		std::unique_ptr<TextureData> data;

		// Direct3D 리소스
		// 셰이더에서 사용할 텍스처 리소스
		ID3D11ShaderResourceView* shaderResourceView = nullptr;

		// 텍스처에서 픽셀을 샘플링하기 위한 속성 객체
		ID3D11SamplerState* samplerState = nullptr;
	};
}
