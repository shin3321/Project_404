#include "GameplayTag/FZFGameplayTags.h"

namespace FZFGameplayTags
{
	// "Input.Action.Interact"라는 태그를 C++ 변수로 정의합니다.
	// 이를 통해 코드에서 문자열 오타 없이 안전하게 태그를 참조할 수 있습니다.
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Interact,"Ability.Action.Interact");

	// 곡괭이 Q키 상호작용.
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_TogglePickaxe,"Ability.Action.TogglePickaxe");

	//아이템 버리기
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_DropItem, "Ability.Action.DropItem");

	// 달리기 / 점프
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Run,"Ability.Action.Run");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Jump, "Ability.Action.Jump");
	
	// 공격
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack, "Ability.Action.Attack");

	// 아이템에 맞는 공격 방식
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Pickaxe, "Ability.Action.Attack.Pickaxe");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Rifle, "Ability.Action.Attack.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Sword, "Ability.Action.Attack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_Pistol, "Ability.Action.Attack.Pistol");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Attack_BarrierStaff, "Ability.Action.Attack.BarrierStaff");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Action_Apply_Revive, "Ability.Action.Apply.Revive");


	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Idle,"State.Movement.Idle");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Walk,"State.Movement.Walk");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Run,"State.Movement.Run");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Jump,"State.Movement.Jump");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Block_Jump,"State.Movement.Block.Jump");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Stamina_NoRegen,"State.Stamina.NoRegen");
	UE_DEFINE_GAMEPLAY_TAG(State_Action_DroppingItem,"State.Action.DroppingItem");
	UE_DEFINE_GAMEPLAY_TAG(State_Action_Attack,"State.Action.Attack");

	// 버프 상태 부여 태그
	UE_DEFINE_GAMEPLAY_TAG(State_Buff_Invincible, "State.Buff.Invincible");

	// 디버프 상태 부여 태그
	UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Stun,"State.Debuff.Stun");

	// 내가 현재 착용하고 있는 무기에 따른 상태
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Pickaxe,"State.Equip.Pickaxe");
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Rifle,"State.Equip.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Pistol,"State.Equip.Pistol");
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_Sword,"State.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(State_Equip_ReviveStone,"State.Equip.ReviveStone");

  	// 어트리뷰트 데이터 초기화용 태그
	UE_DEFINE_GAMEPLAY_TAG(Data_Stat_MaxHp, "Data.Stat.MaxHp");
	UE_DEFINE_GAMEPLAY_TAG(Data_Stat_Hp, "Data.Stat.Hp");
	UE_DEFINE_GAMEPLAY_TAG(Data_Stat_AttackRange, "Data.Stat.AttackRange");
	UE_DEFINE_GAMEPLAY_TAG(Data_Stat_Attack, "Data.Stat.Attack");

	UE_DEFINE_GAMEPLAY_TAG(Data_Move_MovementSpeed, "Data.Move.MovementSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Data_Move_MaxMovementSpeed, "Data.Move.MaxMovementSpeed");

	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_Attack, "Data.Attack.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_MaxAttack, "Data.Attack.MaxAttack");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_AttackRange, "Data.Attack.AttackRange");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_AttackRadius, "Data.Attack.AttackRadius");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_AttackAreaRadius, "Data.Attack.AttackAreaRadius");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_AttackAreaHalfHeight, "Data.Attack.AttackAreaHalfHeight");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_AttackSpeed, "Data.Attack.AttackSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attack_PullStrength, "Data.Attack.PullStrength");

	UE_DEFINE_GAMEPLAY_TAG(Data_BT_DetectRange, "Data.BT.DetectRange");
	UE_DEFINE_GAMEPLAY_TAG(Data_BT_TurnSpeed, "Data.BT.TurnSpeed");

	// 공격 판정 체크
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_AttackHitCheck, "Event.Character.AttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_Ranged_AttackHitCheck, "Event.Character.Ranged.AttackHitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Character_Melee_AttackHitCheck, "Event.Character.Melee.AttackHitCheck");

	UE_DEFINE_GAMEPLAY_TAG(Event_Skill_Barrier_Spawn, "Event.Skill.Barrier.Spawn");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Character_Death, "GameplayCue.Character.Death");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");

	// 무기별 쿨타임 설정
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Attack_Sword, "Cooldown.Attack.Sword");

	// GameplayCue
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Laser, "GameplayCue.Weapon.Laser");
	}