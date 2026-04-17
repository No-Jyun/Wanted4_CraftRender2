#include "CubemapTexture.h"
#include "Graphics/GraphicsContext.h"

namespace Craft
{
#include "Library/StbImage.h"
	CubemapTexture::CubemapTexture()
	{
	}

	CubemapTexture::CubemapTexture(const std::string& path)
	{
		LoadTexture(path);
	}

	void CubemapTexture::LoadTexture(const std::string& path)
	{
		// 6장 이미지 로드.
		const uint32_t imageCount = 6;
		std::vector<std::string> imageNames;
		imageNames.reserve(imageCount);
		std::string baseFolder = "../Assets/Textures/";
		imageNames.emplace_back(baseFolder + path + "/posx.png");
		imageNames.emplace_back(baseFolder + path + "/negx.png");
		imageNames.emplace_back(baseFolder + path + "/posy.png");
		imageNames.emplace_back(baseFolder + path + "/negy.png");
		imageNames.emplace_back(baseFolder + path + "/posz.png");
		imageNames.emplace_back(baseFolder + path + "/negz.png");

		// 이미지 로드 준비.
		//images.reserve(imageCount);
		imageList.reserve(imageCount);

		// data 생성.
		//data = std::make_unique<TextureData>();

		for (const std::string& imageName : imageNames)
		{
			auto& newTextureData
				= imageList.emplace_back(std::make_unique<TextureData>());

			// 이미지 로드.
			newTextureData->pixelArray = stbi_load(
				imageName.c_str(),
				&newTextureData->width,
				&newTextureData->height,
				&newTextureData->channelCount,
				0
			);

			// 예외처리.
			if (!newTextureData->pixelArray)
			{
				ThrowIfFailed(
					E_FAIL, 
					L"Failed to load cubemap image");
			}

			// 3개 채널이면 4채널로 변환.
			if (newTextureData->channelCount == 3)
			{
				ConvertToRGBA(newTextureData);
			}

			// 배열에 추가.
			//images.emplace_back(
			//	reinterpret_cast<byte*>(data->pixelArray)
			//);
		}

		// 리소스 생성.
		CreateSRVAndSampler();
	}

	void CubemapTexture::CreateSRVAndSampler()
	{
		// 셰이더 리소스 생성.
		auto& device = GraphicsContext::Get().GetDevice();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.ArraySize = 6;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		textureDesc.Width = imageList[0]->width;
		textureDesc.Height = imageList[0]->height;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.MipLevels = 1;

		const uint32_t dataCount = 6;
		
		// 이미지에서 로드한 픽셀 정보 설정.
		D3D11_SUBRESOURCE_DATA textureDataList[dataCount] = {};
		for (uint32_t ix = 0; ix < dataCount; ++ix)
		{
			textureDataList[ix].pSysMem = imageList[ix]->pixelArray;
			textureDataList[ix].SysMemPitch 
				= imageList[ix]->width * imageList[ix]->channelCount;
		}

		ID3D11Texture2D* texture = nullptr;
		ThrowIfFailed(device.CreateTexture2D(
			&textureDesc,
			textureDataList,
			&texture
		), L"Failed to create texture2d.");

		// 속성 설정.
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension
			= D3D11_SRV_DIMENSION_TEXTURECUBE;
		shaderResourceViewDesc.TextureCube.MipLevels = 1;

		ThrowIfFailed(device.CreateShaderResourceView(
			texture,
			&shaderResourceViewDesc,
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