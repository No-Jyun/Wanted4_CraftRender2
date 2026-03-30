#include "Shader.h"
#include "Graphics/GraphicsContext.h"
#include "Core/Common.h"
#include <d3dcompiler.h>

namespace Craft
{
	Shader::Shader(const std::wstring& name)
		: name(name)
	{
		auto& device = GraphicsContext::Get().GetDevice();

		std::wstring vertexShaderPath
			= std::wstring(L"../CompiledShaders/") + name;
		vertexShaderPath.append(L"VS.cso");

		ID3DBlob* vertexShaderObject = nullptr;
		//HRESULT result = D3DCompileFromFile(
		//	vertexShaderPath.c_str(),
		//	nullptr,
		//	nullptr,
		//	"main",
		//	"vs_5_0",
		//	0,
		//	0,
		//	&vertexShaderObject,
		//	nullptr
		//);

		HRESULT result = D3DReadFileToBlob(
			vertexShaderPath.c_str(),
			&vertexShaderObject
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		// 셰이더 객체 생성.
		vertexShader = nullptr;
		result = device.CreateVertexShader(
			vertexShaderObject->GetBufferPointer(),
			vertexShaderObject->GetBufferSize(),
			nullptr,
			&vertexShader
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		std::wstring pixelShaderPath = std::wstring(L"../CompiledShaders/") + name;
		pixelShaderPath.append(L"PS.cso");

		ID3DBlob* pixelShaderObject = nullptr;
		//result = D3DCompileFromFile(
		//	pixelShaderPath.c_str(),
		//	nullptr,
		//	nullptr,
		//	"main",
		//	"ps_5_0",
		//	0,
		//	0,
		//	&pixelShaderObject,
		//	nullptr
		//);

		result = D3DReadFileToBlob(
			pixelShaderPath.c_str(),
			&pixelShaderObject
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		// 셰이더 객체 생성.
		pixelShader = nullptr;
		result = device.CreatePixelShader(
			pixelShaderObject->GetBufferPointer(),
			pixelShaderObject->GetBufferSize(),
			nullptr,
			&pixelShader
		);

		if (FAILED(result))
		{
			__debugbreak();
			return;
		}

		// 입력 레이아웃 생성.
		D3D11_INPUT_ELEMENT_DESC inputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 입력 레이아웃 = 정점 셰이더 입력의 명세서.
		// 따라서 정점 셰이더 정보가 있어야 함.
		inputLayout = nullptr;
		result = device.CreateInputLayout(
			inputDesc,
			_countof(inputDesc),
			vertexShaderObject->GetBufferPointer(),
			vertexShaderObject->GetBufferSize(),
			&inputLayout
		);

		SafeRelease(vertexShaderObject);
		SafeRelease(pixelShaderObject);
	}

	Shader::~Shader()
	{
		SafeRelease(vertexShader);
		SafeRelease(pixelShader);
		SafeRelease(inputLayout);
	}

	void Shader::Bind()
	{
		auto& context
			= GraphicsContext::Get().GetDeviceContext();

		// 입력 레이아웃(정점 입력의 명세).
		context.IASetInputLayout(inputLayout);

		// 셰이더 설정.
		context.VSSetShader(vertexShader, nullptr, 0);
		context.PSSetShader(pixelShader, nullptr, 0);
	}
}
