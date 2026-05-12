#include "GameplayTag/FZFGameplayTags.h"

namespace FZFGameplayTags
{
	// "Input.Action.Interact"라는 태그를 C++ 변수로 정의합니다.
	// 이를 통해 코드에서 문자열 오타 없이 안전하게 태그를 참조할 수 있습니다.
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Interact,"Ability.Action.Interact");

	//아이템 버리기
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_DropItem, "Ability.Action.DropItem");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Run,"Ability.Action.Run");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Jump, "Ability.Action.Jump");


	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Idle,"State.Movement.Idle");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Walk,"State.Movement.Walk");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Run,"State.Movement.Run");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Jump,"State.Movement.Jump");
}