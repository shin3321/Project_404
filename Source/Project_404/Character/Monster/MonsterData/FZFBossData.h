// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameplayEffect.h"
#include "FZFBossData.generated.h"

class UAnimSequence;
class UGameplayAbility;
class UAnimInstance;
class UAnimMontage;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct FBossSkillInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UnlockPhase = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsMapPattern = false;

	// 맵 패턴 실행 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Pattern")
	float MapPatternDuration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map Pattern")
	TSubclassOf<UGameplayAbility> AbilityClass;

	// 스킬 사용 전에 MoveTo할 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
	FVector AttackPos = FVector::ZeroVector;

	// 스킬 사용 전에 위/아래 Mesh 이동 목표 Z offset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
	float MeshTargetZOffset = 0.f;

	// 스킬 사용 전에 회전 적용 여부 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotate")
	bool bUseRotateBeforeAttack = false;

	// 스킬 사용 전에 회전(좌우 회전)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotate", meta = (EditCondition = "bUseRotateBeforeAttack"))
	float TargetYawOffset = 0.f;

	// 스킬 사용 전에 회전(상하 회전)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotate", meta = (EditCondition = "bUseRotateBeforeAttack"))
	float TargetMeshPitch = 0.f;
};

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFBossData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Boss Info
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	FName BossId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	TSubclassOf<APawn> BossClass;

	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// 고리 Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	TArray<TObjectPtr<USkeletalMesh>> RingMeshes;

	// 고리 애니메이션 시퀀스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	TArray<TObjectPtr<UAnimSequence>> RingAnim;

	// 고리 애니메이션 재생 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	float RingSpeed;

	// Anim Class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	TSubclassOf<UAnimInstance> AnimClass;

	// Dead Montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	TObjectPtr<UAnimMontage> MonsterDeadMontage;

	// BT
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// Attribute 초기화용 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<UGameplayEffect> InitAttributeEffect;

	// 나머지 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<UGameplayEffect> ChaseSpeedEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TArray<TSubclassOf<UGameplayEffect>> AllowedEffectClasses;

	// Abilities
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	// 판정의 시작점으로 사용할 소켓 이름 (예: Hand_R_Socket, Muzzle_Socket)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket")
	FName AttackSocket;

	// 소켓 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket")
	bool bUseSocketTargeting = true;

	// 보스 스킬 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BossSkill")
	TArray<FBossSkillInfo> SkillList;

	// Attribute 초기화용 데이터들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Stat")
	float MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Move")
	float MaxMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float MaxAttack;

	// 공격 판정 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackRadius;

	// 장판 타격 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackAreaRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackAreaHalfHeight;
	// 공격 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackSpeed; 
	// 당기는 힘
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float PullStrength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|BT")
	float DetectRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|BT")
	float TurnSpeed;
};