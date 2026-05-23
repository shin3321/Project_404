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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;
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
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	//float AttackRange;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	//float AttackRadius;

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