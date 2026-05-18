// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FZFPlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/FZFAbilitySystemComponent.h" 
#include "AbilitySystemInterface.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFPlayerAnimInstance::UFZFPlayerAnimInstance()
{
    bIsRunning = false;
}

void UFZFPlayerAnimInstance::SetUpperBodyBlendWeight(float NewWeight)
{
    UpperBodyBlendWeight = FMath::Clamp(NewWeight, 0.0f, 1.0f);
}

void UFZFPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UFZFPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

    if (IAbilitySystemInterface* ASCHolder = Cast<IAbilitySystemInterface>(Owner))
    {
        UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(ASCHolder->GetAbilitySystemComponent());
        if (ASC)
        {
            // 변수값이 현재 플레이어의 ASC가 보유한 태그에 따라서 설정됨
            bIsRunning = ASC->HasMatchingGameplayTag(FZFGameplayTags::State_Movement_Run);
        }
    }
}
