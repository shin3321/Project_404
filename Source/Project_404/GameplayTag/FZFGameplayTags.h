#pragma once

#include "NativeGameplayTags.h"

// namespace의 사용의미
// 1. FZFGameplayTags::까지만 치면 사용 가능한 모든 태그 목록이 자동 완성되어 협업 시 편리
// 2. FZFGameplayTags::라는 접두사를 통해 우리 프로젝트 전용 태그임을 알 수 있다.
namespace FZFGameplayTags
{
	// .cpp 파일에 정의된 태그를 다른 클래스에서도 참조할 수 있도록 외부에 공개
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Interact);
}