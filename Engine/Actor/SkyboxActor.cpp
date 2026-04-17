#include "SkyboxActor.h"
#include "Resource/MeshLoader.h"
#include "Resource/TextureLoader.h"
#include "Shader/CubemapShader.h"

namespace Craft
{
	SkyboxActor::SkyboxActor()
		: SkyboxActor("Apocalypse")
	{

	}

	SkyboxActor::SkyboxActor(const std::string& path)
	{
		// 스카이 박스로 지정
		isSkybox = true;

		// 모델 로딩
		MeshLoader::Get().GetMesh("Sphere.obj", mesh);

		// 셰이더 생성
		std::shared_ptr<CubemapShader> shader = std::make_shared<CubemapShader>();

		// 큐브맵 로딩 및 설정
		std::weak_ptr<CubemapTexture> cubemapTexture;
		TextureLoader::Get().LoadCubemap(path, cubemapTexture);
		shader->SetTexture(cubemapTexture);

		shaderList.emplace_back(shader);
	}
}