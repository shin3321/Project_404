#include "GameplayTag/FZFGameplayTags.h"

namespace FZFGameplayTags
{
	// "Input.Action.Interact"라는 태그를 C++ 변수로 정의합니다.
	// 이를 통해 코드에서 문자열 오타 없이 안전하게 태그를 참조할 수 있습니다.
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Interact,"Ability.Action.Interact");

	//아이템 버리기
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_DropItem, "Ability.Action.DropItem");

	// 달리기 / 점프
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Run,"Ability.Action.Run");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Jump, "Ability.Action.Jump");
	
	// 공격
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack, "Ability.Action.Attack");

	// 아이템에 맞는 공격 방식
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Rifle, "Ability.Action.Attack.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Sword, "Ability.Action.Attack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Pistol, "Ability.Action.Attack.Pistol");


	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Idle,"State.Movement.Idle");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Walk,"State.Movement.Walk");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Run,"State.Movement.Run");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Jump,"State.Movement.Jump");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_NoRegen,"State.Stamina.NoRegen");
	UE_DEFINE_GAMEPLAY_TAG(State_Action_DroppingItem,"State.Action.DroppingItem");
	UE_DEFINE_GAMEPLAY_TAG(State_Action_Attack,"State.Action.Attack");

	// 내가 현재 착용하고 있는 무기에 따른 상태
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Pickaxe,"State.Equip.Pickaxe");
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Rifle,"State.Equip.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Pistol,"State.Equip.Pistol");
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Swrod,"State.Equip.Sword");


}