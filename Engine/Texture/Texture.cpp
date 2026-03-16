#include "Texture.h"
#include "Graphics/GraphicsContext.h"

namespace Craft
{
#define STB_IMAGE_IMPLEMENTATION
#include "Library/StbImage.h"

	Texture::Texture(const std::string& name, BindType bindType)
		: name(name), bindType(bindType)
	{
		// 데이터 객체 생성
		data = std::make_unique<TextureData>();

		// 이미지 로드 (stb_image 라이브러리 활용)

		// 이미지 애셋 경로
		std::string path = std::string("../Assets/Textures/") + name;

		data->pixelArray = stbi_load(
			path.c_str(),
			&data->width,
			&data->height,
			&data->channelCount,
			0
		);

		// 예외처리
		if (!data->pixelArray)
		{
			__debugbreak();
			return;
		}

		// 셰이더 리소스 생성
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

		// 이미지에서 로드한 픽셀 정보 설정
		D3D11_SUBRESOURCE_DATA textureData = {};
		textureData.pSysMem = data->pixelArray;
		textureData.SysMemPitch = data->width * data->channelCount;

		ID3D11Texture2D* texture = nullptr;
		HRESULT result = device.CreateTexture2D(
			&textureDesc,
			&textureData,
			&texture
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		result = device.CreateShaderResourceView(
			texture,
			nullptr,
			&shaderResouceView
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		SafeRelease(texture);

		// 샘플러 생성
		/*
		    D3D11_FILTER Filter;
		    D3D11_TEXTURE_ADDRESS_MODE AddressU;
		    D3D11_TEXTURE_ADDRESS_MODE AddressV;
		    D3D11_TEXTURE_ADDRESS_MODE AddressW;
		    FLOAT MipLODBias;
		    UINT MaxAnisotropy;
		    D3D11_COMPARISON_FUNC ComparisonFunc;
		    FLOAT BorderColor[ 4 ];
		    FLOAT MinLOD;
		    FLOAT MaxLOD;
		*/
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 3;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;

		result = device.CreateSamplerState(
			&samplerDesc,
			&samplerState
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}
	}

	Texture::~Texture()
	{
		SafeRelease(shaderResouceView);
		SafeRelease(samplerState);
	}

	void Texture::Bind(uint32_t index)
	{
		auto& context = GraphicsContext::Get().GetDeviceContext();

		if (bindType == BindType::VertexShader)
		{
			// 셰이더 단계에 바인딩
			context.VSSetShaderResources(index, 1, &shaderResouceView);

			// 샘플러 바인딩
			context.VSSetSamplers(index, 1, &samplerState);
		}
		else if (bindType == BindType::PixelShader)
		{
			// 셰이더 단계에 바인딩
			context.PSSetShaderResources(index, 1, &shaderResouceView);

			// 샘플러 바인딩
			context.PSSetSamplers(index, 1, &samplerState);
		}
	}
}