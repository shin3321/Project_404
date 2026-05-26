// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_AttackChanneling.generated.h"

UCLASS()
class PROJECT_404_API UAnimNotifyState_AttackChanneling : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_AttackChanneling();

	// 노티파이 구간이 시작될 때 (레이저 시작 프레임)
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	// 노티파이 구간이 끝날 때 (레이저 종료 프레임)
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override { return TEXT("AttackChanneling"); }

protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag StartTriggerTag;

};
