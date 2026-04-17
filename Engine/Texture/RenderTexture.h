#pragma once

#include "Texture.h"

namespace Craft
{
	class CRAFT_API RenderTexture : public Texture
	{
	public:
		RenderTexture(uint32_t width = 0, uint32_t height = 0);
		~RenderTexture();

		// Getter.
		inline ID3D11RenderTargetView* GetRenderTargetView() const
		{
			return renderTargetView;
		}

		inline ID3D11DepthStencilView* GetDepthStencilView() const
		{
			return depthStencilView;
		}

	private:
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
	};
}