#include "RenderTexture.h"
#include "Graphics/GraphicsContext.h"
#include "Core/Engine.h"

namespace Craft
{
	RenderTexture::RenderTexture(uint32_t width, uint32_t height)
	{
		auto& device = GraphicsContext::Get().GetDevice();

		// 너비/높이 값 조정.
		width = width == 0 ? Engine::Get().GetWidth() : width;
		height = height == 0 ? Engine::Get().GetHeight() : height;

		// TextureData 생성.
		data = std::make_unique<TextureData>();
		data->width = width;
		data->height = height;

		// RTV 생성.
		ID3D11Texture2D* renderTargetResource = nullptr;
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.BindFlags
			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		// 생성.
		ThrowIfFailed(device.CreateTexture2D(
			&textureDesc, nullptr, &renderTargetResource),
			L"Failed to create texture2d for render texture");

		// 렌더 타겟 뷰 생성.
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension
			= D3D11_RTV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device.CreateRenderTargetView(
				renderTargetResource,
				&renderTargetViewDesc,
				&renderTargetView),
			L"Failed to create render target view for render texture.");

		// SRV 생성.
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension
			= D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		ThrowIfFailed(
			device.CreateShaderResourceView(
				renderTargetResource,
				&shaderResourceViewDesc,
				&shaderResourceView),
			L"Failed to create shader resrouce view for render texture");

		// 사용한 리소스 해제.
		SafeRelease(renderTargetResource);

		// 샘플러 스테이트 생성.
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

		// 뎁스 스텐실 뷰 생성.
		ID3D11Texture2D* depthStencilBuffer = nullptr;
		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ThrowIfFailed(
			device.CreateTexture2D(
				&depthStencilDesc,
				nullptr,
				&depthStencilBuffer),
			L"Failed to create depth stencil buffer for render texture");

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device.CreateDepthStencilView(
				depthStencilBuffer,
				&depthStencilViewDesc,
				&depthStencilView),
			L"Failed to create depth stencil view for render texture");

		// 사용한 리소스 해제.
		SafeRelease(depthStencilBuffer);
	}

	RenderTexture::~RenderTexture()
	{
		SafeRelease(renderTargetView);
		SafeRelease(depthStencilView);
	}
}