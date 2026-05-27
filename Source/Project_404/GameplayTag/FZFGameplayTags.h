#pragma once

#include "NativeGameplayTags.h"

// namespace의 사용의미
// 1. FZFGameplayTags::까지만 치면 사용 가능한 모든 태그 목록이 자동 완성되어 협업 시 편리
// 2. FZFGameplayTags::라는 접두사를 통해 우리 프로젝트 전용 태그임을 알 수 있다.
namespace FZFGameplayTags
{
	// .cpp 파일에 정의된 태그를 다른 클래스에서도 참조할 수 있도록 외부에 공개
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Interact);

	// 곡괭이 장착 및 해제.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_TogglePickaxe);

	// 아이템 버리기
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_DropItem);

	// 달리기 / 점프
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Run);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Jump);

	// 공격
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack);

	// 아이템에 맞는 공격 방식
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Pickaxe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Rifle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Sword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Pistol);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_BarrierStaff);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Apply_Revive);

	// 로봇 공격 방식
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Robot);


	// 몬스터의 공격 방식
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Attack_Aura);

	// 보스방 디버프
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Action_Boss_Debuff);
	
	// 움직임 상태 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Idle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Walk);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Run);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Block_Jump);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_NoRegen);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_DroppingItem);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Attack);
	
	// 버프 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Buff_Invincible);

	// 디버프 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Stun);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Debuff_InBossRoom);

	// 어트리뷰트 데이터 초기화용 태그
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Stat_MaxHp);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Stat_Hp);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Stat_AttackRange);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Stat_Attack);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Move_MovementSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Move_MaxMovementSpeed);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_MaxAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_AttackRange);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_AttackAreaRadius);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_AttackRadius);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_AttackAreaHalfHeight);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_AttackSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Attack_PullStrength);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_BT_DetectRange);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_BT_TurnSpeed);
	// 내가 현재 착용하고 있는 무기에 따른 상태
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Equip_Pickaxe);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Equip_Rifle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Equip_Pistol);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Equip_Sword);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Equip_ReviveStone);

	// 공격 판정 체크
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_AttackHitCheck);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Ranged_AttackHitCheck);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Character_Melee_AttackHitCheck);

	// 스킬 이벤트
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Skill_Barrier_Spawn);
	
	// 죽음
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	// 쿨타임 설정
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Attack_Sword);

	// GameplayCue
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_Laser);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Weapon_ChargeComplete);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Head_Impact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fireman_Impact);
}
