#include "WarriorCharacter.h"
#include "Resource/MeshLoader.h"
#include "Shader/NormalMappingShader.h"

using namespace Craft;

WarriorCharacter::WarriorCharacter()
{
	// 메시 로드
	MeshLoader::Get().GetMesh("SK_CharM_Warrior.FBX", mesh);
	
	// 셰이더 생성하면서 텍스처 지정
	shaderList.emplace_back(std::make_shared<NormalMappingShader>(
		"T_CharM_WarriorB_D.PNG", "T_Warrior_Emissive.PNG", "T_CharM_Warrior_N.PNG"
	));
}
