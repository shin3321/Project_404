// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameplayEffect.h"
#include "FZFMonsterData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFMonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Monster Info
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Monster)
	FName MonsterId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Monster)
	TSubclassOf<APawn> MonsterClass;

	// Mesh Transform -> BP에서 직접 설정해야 함! 서버 복제할 때 값이 덮어씌워져서 적용 안됨.
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Monster)
	FVector MeshLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Monster)
	FRotator MeshRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Monster)
	FVector MeshScale = FVector(1.f);*/

	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Monster)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	// Anim Class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Monster)
	TSubclassOf<UAnimInstance> AnimClass;

	// BT
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// Attribute 초기화용 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<UGameplayEffect> InitAttributeEffect;

	// 나머지 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TSubclassOf<UGameplayEffect> ChaseSpeedEffect;

	// Abilities
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GAS)
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	// 판정의 시작점으로 사용할 소켓 이름 (예: Hand_R_Socket, Muzzle_Socket)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket")
	FName AttackSocket;

	// 소켓 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Socket")
	bool bUseSocketTargeting = true;

	// Attribute 초기화용 데이터들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Stat")
	float MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Move")
	float MaxMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float MaxAttack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|Attack")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|BT")
	float DetectRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute|BT")
	float TurnSpeed;
};
