// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_AttackChanneling.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UAnimNotifyState_AttackChanneling::UAnimNotifyState_AttackChanneling()
{
}

void UAnimNotifyState_AttackChanneling::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 레이저 시작 구간에 진입하면 보스에게 태그를 던져 채널링 어빌리티를 켭니다!
		FGameplayEventData PayloadData;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), StartTriggerTag, PayloadData);
	}
}

void UAnimNotifyState_AttackChanneling::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
