#include "Texture.h"
#include "Graphics/GraphicsContext.h"

namespace Craft
{
#define STB_IMAGE_IMPLEMENTATION
#include "Library/StbImage.h"

	Texture::Texture(const std::string& name, BindType bindType)
		: name(name), bindType(bindType)
	{
		LoadTexture(name);
	}

	Texture::~Texture()
	{
		SafeRelease(shaderResourceView);
		SafeRelease(samplerState);
	}

	void Texture::Bind(uint32_t index)
	{
		auto& context = GraphicsContext::Get().GetDeviceContext();

		if (bindType == BindType::VertexShader)
		{
			// 셰이더 단계에 바인딩
			context.VSSetShaderResources(index, 1, &shaderResourceView);

			// 샘플러 바인딩
			context.VSSetSamplers(index, 1, &samplerState);
		}
		else if (bindType == BindType::PixelShader)
		{
			// 셰이더 단계에 바인딩
			context.PSSetShaderResources(index, 1, &shaderResourceView);

			// 샘플러 바인딩
			context.PSSetSamplers(index, 1, &samplerState);
		}
	}
	void Texture::ConvertToRGBA(std::unique_ptr<TextureData>& textureData)
	{
		// 목표 채널 수 (4개)
		const uint32_t targetChannelCount = 4;

		// 픽셀 수
		const uint32_t pixelCount = textureData->width * textureData->height;

		// 이미지 버퍼 크기 = 픽셀 수 x 채널 수 (버퍼 생성 용)
		const uint32_t size = pixelCount * targetChannelCount;

		// 새로운 이미지 버퍼 생성
		uint8_t* imageBuffer = new uint8_t[size];

		// 모든 데이터를 초기화
		memset(imageBuffer, 255, size);

		// 원본 이미지 데이터 포인터
		uint8_t* source = reinterpret_cast<uint8_t*>(textureData->pixelArray);

		// 새로운 이미지 데이터를 생성할 버퍼 포인터
		uint8_t* dest = imageBuffer;

		// 기준 이미지 데이터 + 알파 채널 추가 처리
		for (uint32_t ix = 0; ix < pixelCount; ix++)
		{
			// 메모리 복사 (3바이트 만큼만 복사)
			memcpy(dest, source, sizeof(uint8_t) * 3);

			//dest[0] = source[0];
			//dest[1] = source[1];
			//dest[2] = source[2];

			//dest[3] = 255;

			// 다음 위치로 메모리 이동
			source += 3;
			dest += 4;
		}

		// 기존에 로드한 픽셀 정보 해제
		if (textureData->pixelArray)
		{
			free(textureData->pixelArray);
			textureData->pixelArray = nullptr;
		}

		// 새로 생성한 이미지 데이터로 설정
		textureData->pixelArray = imageBuffer;
		textureData->channelCount = 4;
	}

	void Texture::LoadTexture(const std::string& name)
	{
		// 데이터 객체 생성.
		data = std::make_unique<TextureData>();

		// 이미지 로드(stb_image 라이브러리 활용).

		// 이미지 애셋 경로.
		std::string path
			= std::string("../Assets/Textures/") + name;

		data->pixelArray = stbi_load(
			path.c_str(),
			&data->width,
			&data->height,
			&data->channelCount,
			0
		);

		// 24비트(채널 수 3개) 텍스처인 경우 32비트로 변환.
		if (data->channelCount == 3)
		{
			ConvertToRGBA(data);
		}

		// 예외 처리.
		if (!data->pixelArray)
		{
			__debugbreak();
			return;
		}

		// SRV 및 Sampler 생성.
		CreateSRVAndSampler();
	}

	void Texture::CreateSRVAndSampler()
	{
		// 셰이더 리소스 생성.
		auto& device = GraphicsContext::Get().GetDevice();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.ArraySize = 1;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Width = data->width;
		textureDesc.Height = data->height;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.MipLevels = 1;

		// 이미지에서 로드한 픽셀 정보 설정.
		D3D11_SUBRESOURCE_DATA textureData = {};
		textureData.pSysMem = data->pixelArray;
		textureData.SysMemPitch = data->width * data->channelCount;

		ID3D11Texture2D* texture = nullptr;
		ThrowIfFailed(device.CreateTexture2D(
			&textureDesc,
			&textureData,
			&texture
		), L"Failed to create texture2d.");

		ThrowIfFailed(device.CreateShaderResourceView(
			texture,
			nullptr,
			&shaderResourceView
		), L"Failed to create shader resource view.");

		SafeRelease(texture);

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 3;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;

		ThrowIfFailed(device.CreateSamplerState(
			&samplerDesc,
			&samplerState
		), L"Failed to create sampler state");
	}
}